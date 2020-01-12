#ifndef GRAPH_CUT_H
#define GRAPH_CUT_H

#include <opencv2/opencv.hpp>

class GraphCut
{

public:

    // Performance Metric
    struct PerfMetric
    {
        double m_tNLink;
        double m_tTLink;
        double m_tMaxFlow;

        PerfMetric &operator=(const PerfMetric &rhs)
        {
            m_tNLink = rhs.m_tNLink;
            m_tTLink = rhs.m_tTLink;
            m_tMaxFlow = rhs.m_tMaxFlow;
            return (*this);
        }
    };

public:

    static void cutImage(cv::InputArray image, cv::OutputArray result, cv::OutputArray mask
        , const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds
        , float lambda = 0.01f, float sigma = -1.0, PerfMetric* perfMetric = nullptr
    );

};

#endif // GRAPH_CUT_H