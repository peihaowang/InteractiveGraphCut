
#include <iostream>
#include <opencv2/opencv.hpp>
#include "GraphCut.h"

void mask2Vec(const cv::Mat& img, std::vector<cv::Point>& fore, std::vector<cv::Point>& back){
    for(int y = 0; y < img.rows; y++){
        for(int x = 0; x < img.cols; x++){
            const unsigned char* p = img.ptr<unsigned char>(y, x);
            if(p[2] >= 200) fore.push_back(cv::Point(x, y));
            else if(p[1] >= 200) back.push_back(cv::Point(x, y));
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 5){
        std::cout << "Usage: GraphCutter <input image> <input seed> <output path> <mask path>" << std::endl;
        return -1;
    }

    cv::Mat imgSrc = cv::imread(argv[1]);
    cv::Mat imgSeed = cv::imread(argv[2]);

    std::vector<cv::Point> fore, back;
    mask2Vec(imgSeed, fore, back);

    cv::Mat result, mask;
    GraphCut::PerfMetric perf;
    GraphCut::cutImage(imgSrc, result, mask, fore, back, 0.5, -1.0, &perf);
    cv::imwrite(argv[3], result);
    cv::imwrite(argv[4], mask);

    std::cout << "Create N-Links: " << perf.m_tNLink << "s" << std::endl;
    std::cout << "Create T-Links: " << perf.m_tTLink << "s" << std::endl;
    std::cout << "Max Flow: " << perf.m_tMaxFlow << "s" << std::endl;

    return 0;
}
