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

    /// @brief Visualize keypoints on image
    /// @param image input image (gray or color)
    /// @param keypoints detected keypoints
    /// @return image with drawn detections
    cv::Mat visualize(const cv::Mat& image, const std::vector<cv::KeyPoint>& keypoints) const;
};

#endif