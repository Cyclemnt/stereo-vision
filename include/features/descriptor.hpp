#ifndef __DESCRIPTOR__
#define __DESCRIPTOR__

#include <vector>
#include <opencv2/opencv.hpp>

class Descriptor {
private:
    cv::Ptr<cv::ORB> orb;
public:
    Descriptor(/* args */);

    /// @brief Computes feature descriptors
    /// @param image Image
    /// @param keypoints Keypoints
    /// @return A cv::Mat of feature descriptors
    cv::Mat compute(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints) const;
};

#endif