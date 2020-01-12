
#include <math.h>

#include <opencv2/opencv.hpp>

#include "StopWatch.h"
#include "MaxFlow.h"
#include "Histogram.h"
#include "GraphCutImpl.h"

AbstractGraphCutImpl::AbstractGraphCutImpl(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds)
    : MaxFlow(image.cols() * image.rows() + 2), m_foreSeeds(foreSeeds), m_backSeeds(backSeeds)
{
    m_image = makeContinuous(image.getMat());

    int countOfPixels = image.cols() * image.rows();
    MaxFlow::setTerminals(countOfPixels, countOfPixels + 1);

    m_adjacenceIndex.resize(numberOfVertices());
    for(int u = 0; u < numberOfVertices(); u++){
        Adjacence**& adjacences = m_adjacenceIndex[u];
        int countOfAdjacences = -1;
        if(MaxFlow::isTerminal(u)){
            // Here we define, the index of adjacence equals to the corresponding index of pixel
            countOfAdjacences = countOfPixels;
        }else{
            // Here we define, 0 ~ 3 correspond to left, top, right, bottom respectively
            // 4, 5 correspond to source and sink respectively
            countOfAdjacences = 4 + 2;   // Four neighbors and two edges connecting to terminals
        }
        adjacences = new Adjacence*[countOfAdjacences];
        for(int i = 0; i < countOfAdjacences; i++) adjacences[i] = nullptr;
    }
}

AbstractGraphCutImpl::~AbstractGraphCutImpl()
{
    for(int i = 0; i < numberOfVertices(); i++){
        Adjacence** adjacences = m_adjacenceIndex[i];
        delete[] adjacences; adjacences = nullptr;
    }
}

cv::Mat AbstractGraphCutImpl::makeContinuous(const cv::Mat &m)
{
    if (!m.isContinuous()) {
        return m.clone();
    }
    return m;
}

AbstractGraphCutImpl::Adjacence **AbstractGraphCutImpl::adjacencePointer(Vertex u, Vertex v) const
{
    Adjacence** result = nullptr;
    if(Vertex(0) <= u && u < Vertex(numberOfVertices())){
        Adjacence** adjacences = m_adjacenceIndex[u];
        if(MaxFlow::isTerminal(u)){
            if(!MaxFlow::isTerminal(v)){
                result = &adjacences[v];
            }
        }else{
            Vertex neighbors[] = { u - 1, u - m_image.cols, u + 1, u + m_image.cols, source(), sink() };
            for(int i = 0; i < sizeof(neighbors)/sizeof(Vertex); i++){
                if(v == neighbors[i]){
                    result = &adjacences[i];
                    break;
                }
            }
        }
    }
    return result;
}

AbstractGraphCutImpl::Adjacence *AbstractGraphCutImpl::addEdge(Vertex u, Vertex v, Weight w)
{
    Adjacence* adjacence = MaxFlow::addEdge(u, v, w);
    updateAdjacenceIndex(u, v, adjacence);
    return adjacence;
}

bool AbstractGraphCutImpl::removeEdge(Vertex u, Vertex v)
{
    bool succ = MaxFlow::removeEdge(u, v);
    if(succ) updateAdjacenceIndex(u, v, nullptr);
    return succ;
}

float AbstractGraphCutImpl::createNEdges()
{
    float maxPenalty = 0.0f;
    for(int i = 0; i < m_image.cols * m_image.rows; i++){
        int neighbor[] = { i - 1, i - m_image.cols, i + 1, i + m_image.cols };
        for(int j = 0; j < sizeof(neighbor) / sizeof(int); j++){
            cv::Point coord = coordOfIndex(neighbor[j]);
            if(0 <= coord.x && coord.x < m_image.cols
                && 0 <= coord.y && coord.y < m_image.rows
            ){
                float B = smoothPenalty(i, neighbor[j]);
                MaxFlow::setEdge(i, neighbor[j], B);
                if(maxPenalty < B) maxPenalty = B;
            }
        }
    }
    return maxPenalty;
}

void AbstractGraphCutImpl::createTEdges(float K)
{
    MaxFlow::Vertex source = MaxFlow::source(), sink = MaxFlow::sink();

    // Initiate indexing set
    std::set<MaxFlow::Vertex> foreSet, backSet;
    for(cv::Point coord : m_foreSeeds) foreSet.insert(indexOfCoord(coord));
    for(cv::Point coord : m_backSeeds) backSet.insert(indexOfCoord(coord));

    // Setup T-links
    for(int i = 0; i < m_image.cols * m_image.rows; i++){
        if(foreSet.find(i) != foreSet.end()){
            MaxFlow::setEdge(source, i, K);
            MaxFlow::setEdge(i, sink, 0.0f);
        }else if(backSet.find(i) != backSet.end()){
            MaxFlow::setEdge(source, i, 0.0f);
            MaxFlow::setEdge(i, sink, K);
        }else{
            float Rp[] = { backgroundPenalty(i), foregroundPenalty(i) };
            MaxFlow::setEdge(source, i, Rp[0]);
            MaxFlow::setEdge(i, sink, Rp[1]);
        }
    }
}

void AbstractGraphCutImpl::generateMask(cv::OutputArray result)
{
    // Execute Ford-Fulkerson algorithm
    cv::Mat mask = cv::Mat::zeros(m_image.rows, m_image.cols, CV_8UC1);
    std::set<MaxFlow::Vertex> foreground, background;
    MaxFlow::minCut(foreground, background);
    for(MaxFlow::Vertex v : foreground){
        cv::Point coord = coordOfIndex(v);
        (*mask.ptr<unsigned char>(coord.y, coord.x)) = 255;
    }
    mask.copyTo(result);
}

void AbstractGraphCutImpl::cutImage(cv::InputArray image, cv::OutputArray result, cv::OutputArray mask)
{
    StopWatch timer;

    // Construct edges
    float maxPenalty = createNEdges();
    m_perfMetric.m_tNLink = timer.tick();

    createTEdges(maxPenalty + 1.0f);
    m_perfMetric.m_tTLink = timer.tick();

    // Generate mask
    generateMask(mask);
    m_perfMetric.m_tMaxFlow = timer.tick();

    image.getMat().copyTo(result, mask);
}

///////////////////////////////////////////////////////////////////////////////

GraphCutImpl::GraphCutImpl(cv::InputArray image, const std::vector<cv::Point> &foreSeeds, const std::vector<cv::Point> &backSeeds)
    : AbstractGraphCutImpl(image, foreSeeds, backSeeds)
{
    m_foreDistribution = new Histogram3c(0, 255, 10);
    m_backDistribution = new Histogram3c(0, 255, 10);
    samplingImage();

    // Initiate constant parameters
    m_Lambda = 0.01f;
    m_Sigma = computeNoise();
}

GraphCutImpl::~GraphCutImpl()
{
    delete m_foreDistribution;
    m_foreDistribution = nullptr;

    delete m_backDistribution;
    m_backDistribution = nullptr;
}

float GraphCutImpl::pixelDifference(unsigned char values1[3], unsigned char values2[3])
{
    return sqrt(pow(values1[0] - values2[0], 2) + pow(values1[1] - values2[1], 2) + pow(values1[2] - values2[2], 2));
}

float GraphCutImpl::computeNoise()
{
    int numOfEdges = 0; float totalDiff = 0.0f;
    for(int i = 0; i < m_image.cols * m_image.rows; i++){
        cv::Point coord1 = coordOfIndex(i);
        int neighbor[] = { i - 1, i - m_image.cols, i + 1, i + m_image.cols };
        for(int j = 0; j < sizeof(neighbor) / sizeof(int); j++){
            cv::Point coord2 = coordOfIndex(neighbor[j]);
            if(0 <= coord2.x && coord2.x < m_image.cols
                && 0 <= coord2.y && coord2.y < m_image.rows
            ){
                unsigned char* values1 = m_image.ptr<unsigned char>(coord1.y, coord1.x);
                unsigned char* values2 = m_image.ptr<unsigned char>(coord2.y, coord2.x);
                float pixelDiff = pixelDifference(values1, values2);
                totalDiff += pixelDiff;
                numOfEdges++;
            }
        }
    }
    return totalDiff / (float)numOfEdges;
}

void GraphCutImpl::samplingImage()
{
    for(cv::Point coord : m_foreSeeds){
        unsigned char* pixel = m_image.ptr<unsigned char>(coord.y, coord.x);
        m_foreDistribution->accumulate(pixel);
    }
    for(cv::Point coord : m_backSeeds){
        unsigned char* pixel = m_image.ptr<unsigned char>(coord.y, coord.x);
        m_backDistribution->accumulate(pixel);
    }
}

float GraphCutImpl::foregroundPenalty(int pixel)
{
    cv::Point coord = coordOfIndex(pixel);
    unsigned char* values = m_image.ptr<unsigned char>(coord.y, coord.x);
    float f = (double)m_foreDistribution->frequency(values) / (double)m_foreDistribution->total();
    if(f <= 0.0) f = 10e-6;
    float penalty = -m_Lambda * log(f);
    return penalty;
}

float GraphCutImpl::backgroundPenalty(int pixel)
{
    cv::Point coord = coordOfIndex(pixel);
    unsigned char* values = m_image.ptr<unsigned char>(coord.y, coord.x);
    float f = (double)m_backDistribution->frequency(values) / (double)m_backDistribution->total();
    if(f <= 0.0) f = 10e-6;
    float penalty = -m_Lambda * log(f);
    return penalty;
}

float GraphCutImpl::smoothPenalty(int pixel, int neighbor)
{
    cv::Point coord1 = coordOfIndex(pixel), coord2 = coordOfIndex(neighbor);
    unsigned char* values1 = m_image.ptr<unsigned char>(coord1.y, coord1.x);
    unsigned char* values2 = m_image.ptr<unsigned char>(coord2.y, coord2.x);
    float pixelDiff = pixelDifference(values1, values2);
    return exp(-pow(pixelDiff, 2) / (2.0*m_Sigma*m_Sigma));
}
