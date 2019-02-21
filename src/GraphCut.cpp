
#include <math.h>

#include <opencv2/opencv.hpp>

#include "StopWatch.h"
#include "MaxFlow.h"
#include "Histogram.h"
#include "GraphCut.h"

AbstractGraphCut::AbstractGraphCut(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds)
    : m_foreSeeds(foreSeeds)
    , m_backSeeds(backSeeds)
{
    m_image = makeContinuous(image.getMat());

    int countOfPixels = image.cols() * image.rows();
    m_flowNetwork = new MaxFlow(countOfPixels + 2, countOfPixels, countOfPixels + 1);
}

AbstractGraphCut::~AbstractGraphCut()
{
    delete m_flowNetwork;
    m_flowNetwork = nullptr;
}

cv::Mat AbstractGraphCut::makeContinuous(const cv::Mat& m)
{
    if (!m.isContinuous()) {
        return m.clone();
    }
    return m;
}

float AbstractGraphCut::createNEdges()
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
                // std::cout << "smooth:" << B << std::endl;
                m_flowNetwork->addEdge(i, neighbor[j], B);
                if(maxPenalty < B) maxPenalty = B;
            }
        }
    }
    return maxPenalty;
}

void AbstractGraphCut::createTEdges(float K)
{
    MaxFlow::Vertex source = m_flowNetwork->source(), sink = m_flowNetwork->sink();

    // Initiate indexing set
    std::set<MaxFlow::Vertex> foreSet, backSet;
    for(cv::Point coord : m_foreSeeds) foreSet.insert(indexOfCoord(coord));
    for(cv::Point coord : m_backSeeds) backSet.insert(indexOfCoord(coord));

    // Uniformly setup N-links
    for(int i = 0; i < m_image.cols * m_image.rows; i++){
        if(foreSet.find(i) != foreSet.end()){
            m_flowNetwork->addEdge(source, i, K);
            m_flowNetwork->addEdge(i, sink, 0.0f);
        }else if(backSet.find(i) != backSet.end()){
            m_flowNetwork->addEdge(source, i, 0.0f);
            m_flowNetwork->addEdge(i, sink, K);
        }else{
            float Rp[] = { backgroundPenalty(i), foregroundPenalty(i) };
            // std::cout << "Rp:" << Rp[0] << " " << Rp[1] << std::endl;
            m_flowNetwork->addEdge(source, i, Rp[0]);
            m_flowNetwork->addEdge(i, sink, Rp[1]);
        }
    }
}

void AbstractGraphCut::generateMask(cv::OutputArray result)
{
    // Execute Ford-Fulkerson algorithm
    cv::Mat mask = cv::Mat::zeros(m_image.rows, m_image.cols, CV_8UC1);
    std::set<MaxFlow::Vertex> foreground, background;
    m_flowNetwork->minCut(foreground, background);
    for(MaxFlow::Vertex v : foreground){
        cv::Point coord = coordOfIndex(v);
        (*mask.ptr<unsigned char>(coord.y, coord.x)) = 255;
    }
    mask.copyTo(result);
}

void AbstractGraphCut::cutImage(cv::InputArray image, cv::OutputArray result)
{
    StopWatch sw;

    // Construct edges
    float maxPenalty = createNEdges();
    sw.tick("Create N-Link Edges");

    createTEdges(maxPenalty + 1.0f);
    sw.tick("Create T-Link Edges");

    // Generate mask
    cv::Mat mask; generateMask(mask);
    sw.tick("Ford-Fulkerson");

    image.getMat().copyTo(result, mask);
    sw.tick("Cut Image");
}

///////////////////////////////////////////////////////////////////////////////

GraphCut::GraphCut(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds)
    : AbstractGraphCut(image, foreSeeds, backSeeds)
{
    m_foreDistribution = new Histogram3c(0, 255, 10);
    m_backDistribution = new Histogram3c(0, 255, 10);
    samplingImage();

    // Initiate constant parameters
    m_Lambda = 0.01f;
    m_Sigma = computeNoise();
}

GraphCut::~GraphCut()
{
    delete m_foreDistribution;
    m_foreDistribution = nullptr;

    delete m_backDistribution;
    m_backDistribution = nullptr;
}

float GraphCut::pixelDifference(unsigned char values1[3], unsigned char values2[3])
{
    return sqrt(pow(values1[0] - values2[0], 2) + pow(values1[1] - values2[1], 2) + pow(values1[2] - values2[2], 2));
}

float GraphCut::computeNoise()
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

void GraphCut::samplingImage()
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

float GraphCut::foregroundPenalty(int pixel)
{
    cv::Point coord = coordOfIndex(pixel);
    unsigned char* values = m_image.ptr<unsigned char>(coord.y, coord.x);
    float f = (double)m_foreDistribution->frequency(values) / (double)m_foreDistribution->total();
    if(f <= 0.0) f = 10e-6;
    // std::cout << f << " " << log(f) << " " << (double)m_foreDistribution->frequency(values) << " " << (double)m_foreDistribution->total() << std::endl;
    float penalty = -m_Lambda * log(f);
    return penalty;
}

float GraphCut::backgroundPenalty(int pixel)
{
    cv::Point coord = coordOfIndex(pixel);
    unsigned char* values = m_image.ptr<unsigned char>(coord.y, coord.x);
    float f = (double)m_backDistribution->frequency(values) / (double)m_backDistribution->total();
    if(f <= 0.0) f = 10e-6;
    float penalty = -m_Lambda * log(f);
    return penalty;
}

float GraphCut::smoothPenalty(int pixel, int neighbor)
{
    cv::Point coord1 = coordOfIndex(pixel), coord2 = coordOfIndex(neighbor);
    unsigned char* values1 = m_image.ptr<unsigned char>(coord1.y, coord1.x);
    unsigned char* values2 = m_image.ptr<unsigned char>(coord2.y, coord2.x);
    float pixelDiff = pixelDifference(values1, values2);
    return exp(-pow(pixelDiff, 2) / (2.0*m_Sigma*m_Sigma));
}

void GraphCut::cutImage(cv::InputArray image, cv::OutputArray result, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds, float lambda, float sigma)
{
    GraphCut GC(image, foreSeeds, backSeeds);

    // Initiate parameters
    if(lambda >= 0.0) GC.m_Lambda = lambda;
    if(sigma >= 0.0) GC.m_Sigma = sigma;

    GC.cutImage(image, result);
}
