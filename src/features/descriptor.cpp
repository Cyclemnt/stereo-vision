#include "features/descriptor.hpp"

Descriptor::Descriptor()
    : sift(cv::SIFT::create()) {}

cv::Mat Descriptor::compute(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints) const {
    cv::Mat descriptors;

    // Compute descriptors
    sift->compute(image, keypoints, descriptors);
    // There is also a function that detects AND computes feature descriptors:
    // sift->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

    return descriptors;
}