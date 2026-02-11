#include "features/descriptor.hpp"

Descriptor::Descriptor()
    : orb(cv::ORB::create()) {}

cv::Mat Descriptor::compute(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints) const {
    cv::Mat descriptors;

    // Compute descriptors
    orb->compute(image, keypoints, descriptors);

    // There is also a function that detects AND computes feature descriptors:
    // orb->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

    return descriptors;
}