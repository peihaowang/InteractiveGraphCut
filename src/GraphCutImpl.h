#ifndef _GRAPH_CUT_IMPL_H_
#define _GRAPH_CUT_IMPL_H_

#include "MaxFlow.h"
#include "Histogram.h"
#include "GraphCut.h"

#include <opencv2/opencv.hpp>

class AbstractGraphCutImpl : private MaxFlow
{

protected:

    cv::Mat                     m_image;
    std::vector<cv::Point>      m_foreSeeds;
    std::vector<cv::Point>      m_backSeeds;

    std::vector<Adjacence**>    m_adjacenceIndex;

    GraphCut::PerfMetric        m_perfMetric;

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

    AbstractGraphCutImpl(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds);
    virtual ~AbstractGraphCutImpl();

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

    void cutImage(cv::InputArray image, cv::OutputArray result, cv::OutputArray mask);

};

///////////////////////////////////////////////////////////////////////////////

class GraphCutImpl : private AbstractGraphCutImpl
{

protected:

    float                       m_Sigma;
    float                       m_Lambda;

    Histogram3c*                m_foreDistribution;
    Histogram3c*                m_backDistribution;

    friend class GraphCut;

protected:

    GraphCutImpl(cv::InputArray image, const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds);
    virtual ~GraphCutImpl();

    static float pixelDifference(unsigned char values1[3], unsigned char values2[3]);

    void samplingImage();
    float computeNoise();

    virtual float foregroundPenalty(int pixel) override;
    virtual float backgroundPenalty(int pixel) override;
    virtual float smoothPenalty(int pixel, int neighbor) override;

    using AbstractGraphCutImpl::cutImage;

};

#endif // _GRAPH_CUT_IMPL_H_
