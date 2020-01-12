#include <iostream>
#include "StopWatch.h"
#include "GraphCutImpl.h"
#include "GraphCut.h"

void GraphCut::cutImage(cv::InputArray image, cv::OutputArray result, cv::OutputArray mask
    , const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds
    , float lambda, float sigma, PerfMetric* perfMetric
) {
    GraphCutImpl GC(image, foreSeeds, backSeeds);

    // Initiate parameters
    if (lambda >= 0.0) GC.m_Lambda = lambda;
    if (sigma >= 0.0) GC.m_Sigma = sigma;

    // Run graph cut
    GC.cutImage(image, result, mask);

    // Fill out the perfermance metric
    if (perfMetric) {
        (*perfMetric) = GC.m_perfMetric;
    }
}
