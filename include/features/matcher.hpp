#ifndef __MATCHER__
#define __MATCHER__

#include <vector>
#include <opencv2/opencv.hpp>

class Matcher {
private:
    cv::Ptr<cv::BFMatcher> matcher;

public:
    Matcher(/* args */);

    /// @brief Matches two sets of feature descriptors
    /// @param descriptors1 First set of descriptors
    /// @param descriptors2 Second set of descriptors
    /// @return A vector of descriptor matches
    std::vector<cv::DMatch> match(cv::Mat& descriptors1, cv::Mat& descriptors2) const;

    /// @brief Visualize matches between two images
    /// @param img1 First image
    /// @param kp1 Keypoints from image 1
    /// @param img2 Second image
    /// @param kp2 Keypoints from image 2
    /// @param matches Matches between descriptors
    /// @return Image showing matches
    cv::Mat visualize(const cv::Mat& img1, const std::vector<cv::KeyPoint>& kp1, const cv::Mat& img2, const std::vector<cv::KeyPoint>& kp2, const std::vector<cv::DMatch>& matches) const;
};

#endif