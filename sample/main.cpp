
#include <iostream>
#include <opencv2/opencv.hpp>
#include "GraphCut.h"

int main(int argc, char* argv[])
{
    cv::Mat imgSeed = cv::imread("../ironman_mask.jpg");
    cv::Mat imgSrc = cv::imread("../ironman.jpg");
    auto mask2Vec = [](const cv::Mat& img, std::vector<cv::Point>& fore, std::vector<cv::Point>& back){
        for(int y = 0; y < img.rows; y++){
            for(int x = 0; x < img.cols; x++){
                const unsigned char* p = img.ptr<unsigned char>(y, x);
                if(p[2] >= 200) fore.push_back(cv::Point(x, y));
                else if(p[1] >= 200) back.push_back(cv::Point(x, y));
            }
        }
    };
    std::vector<cv::Point> fore, back; mask2Vec(imgSeed, fore, back);
    std::cout << fore.size() << std::endl;
    std::cout << back.size() << std::endl;
    // cv::Mat mask = cv::Mat::zeros(imgSrc.rows, imgSrc.cols, CV_8UC1);
    // for(cv::Point v : fore){
    //     (*mask.ptr<unsigned char>(v.y, v.x)) = 255;
    // }
    // cv::imwrite("../mask.jpg", mask);

    cv::Mat result;
    GraphCut::cutImage(imgSrc, result, fore, back, 0.5);
    cv::imwrite("../ironman_result.jpg", result);
    return 0;
}
