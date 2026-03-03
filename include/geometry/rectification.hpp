#ifndef __RECTIFICATION__
#define __RECTIFICATION__

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <string>

class StereoRectifier {
private:
    // lookup tables
    cv::Mat mapL1, mapL2;
    cv::Mat mapR1, mapR2;

public:
    StereoRectifier(const cv::Mat& K1, const cv::Mat& D1,
                    const cv::Mat& K2, const cv::Mat& D2,
                    const cv::Mat& R,  const cv::Mat& T,
                    cv::Size imgSize);

    /// @brief Apply rectification on both images
    void process(const cv::Mat& imgL, const cv::Mat& imgR, cv::Mat& outL, cv::Mat& outR);
};

#endif