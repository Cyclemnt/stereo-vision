#ifndef __RECTIFICATION__
#define __RECTIFICATION__

#include <opencv2/opencv.hpp>
#include <string>

class StereoRectifier {
private:
    // lookup tables
    cv::Mat mapL1, mapL2;
    cv::Mat mapR1, mapR2;

public:
    StereoRectifier();

    /// @brief Initialize the lookup tables
    void init();

    /// @brief Apply rectification on both images
    void process(const cv::Mat& imgL, const cv::Mat& imgR, cv::Mat& outL, cv::Mat& outR);
};

#endif