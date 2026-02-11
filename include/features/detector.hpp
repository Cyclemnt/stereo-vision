#ifndef __DETECTOR__
#define __DETECTOR__

#include <vector>
#include <opencv2/opencv.hpp>

class Detector {
public:
    Detector();

    /// @brief Harris Corner Detection
    /// @param gray grayscale image
    /// @return A vector of detected keypoints
    std::vector<cv::KeyPoint> detect(cv::Mat& gray) const;
};

#endif