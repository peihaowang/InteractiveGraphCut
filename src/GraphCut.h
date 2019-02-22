
#ifndef _GRAPH_CUT_H_
#define _GRAPH_CUT_H_

#include "MaxFlow.h"
#include "Histogram.h"

class AbstractGraphCut : private MaxFlow
{

protected:

    cv::Mat                     m_image;
    std::vector<cv::Point>      m_foreSeeds;
    std::vector<cv::Point>      m_backSeeds;

    std::vector<Adjacence**>    m_adjacenceIndex;

protected:

    using Graph::addEdge;
    using Graph::removeEdge;

    Adjacence** adjacencePointer(Vertex u, Vertex v) const;

    bool updateAdjacenceIndex(Vertex u, Vertex v, Adjacence* adjacence)
    {
        bool succ = false;
        Adjacence** p = adjacencePointer(u, v);
        if(p){
            (*p) = adjacence;
            succ = true;
        }
        return succ;
    }

    virtual Adjacence* adjacenceOf(Vertex u, Vertex v) const override
    {
        Adjacence** p = adjacencePointer(u, v);
        return p ? (*p) : nullptr;
    }

    virtual Adjacence* addEdge(Vertex u, Vertex v, Weight w) override;
    virtual bool removeEdge(Vertex u, Vertex v) override;

protected:

    AbstractGraphCut(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds);
    virtual ~AbstractGraphCut();

    static cv::Mat makeContinuous(const cv::Mat& m);

    cv::Point coordOfIndex(int idx) const { return cv::Point(idx % m_image.cols, floor(idx / m_image.cols)); }
    int indexOfCoord(const cv::Point& coord) const { return coord.y * m_image.cols + coord.x; }

    virtual float foregroundPenalty(int pixel) = 0;
    virtual float backgroundPenalty(int pixel) = 0;
    virtual float smoothPenalty(int pixel, int neighbor) = 0;

    // createNEdges will calculate the maximum penalty while adding edges
    // for creation of T-links requires it
    float createNEdges();
    // K is the set to seed nodes for hard constrains
    void createTEdges(float K);

    void generateMask(cv::OutputArray result);

public:

    void cutImage(cv::InputArray image, cv::OutputArray result);


};

///////////////////////////////////////////////////////////////////////////////

class GraphCut : private AbstractGraphCut
{

protected:

    float                       m_Sigma;
    float                       m_Lambda;

    Histogram3c*                m_foreDistribution;
    Histogram3c*                m_backDistribution;

protected:

    GraphCut(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds);
    virtual ~GraphCut();

    static float pixelDifference(unsigned char values1[3], unsigned char values2[3]);

    void samplingImage();
    float computeNoise();

    virtual float foregroundPenalty(int pixel) override;
    virtual float backgroundPenalty(int pixel) override;
    virtual float smoothPenalty(int pixel, int neighbor) override;

    using AbstractGraphCut::cutImage;

public:

    static void cutImage(cv::InputArray image, cv::OutputArray result, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds, float lambda = -1.0, float sigma = -1.0);

};

#endif // _GRAPH_CUT_H_
