#include "features/matcher.hpp"

Matcher::Matcher()
    : matcher(cv::BFMatcher::create(cv::NORM_HAMMING, true)) {}

std::vector<cv::DMatch> Matcher::match(cv::Mat& descriptors1, cv::Mat& descriptors2) const {
    std::vector<cv::DMatch> matches;

    // Match descriptors
    matcher->match(descriptors1, descriptors2, matches);

    return matches;
}

cv::Mat Matcher::visualize(const cv::Mat& img1, const std::vector<cv::KeyPoint>& kp1, const cv::Mat& img2, const std::vector<cv::KeyPoint>& kp2, const std::vector<cv::DMatch>& matches) const {
    cv::Mat output;

    cv::drawMatches(
        img1, kp1,
        img2, kp2,
        matches,
        output,
        cv::Scalar::all(-1),  // auto color
        cv::Scalar::all(-1),
        std::vector<char>(),
        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
    );

    return output;
}
