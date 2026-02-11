#include "features/detector.hpp"

Detector::Detector() {}

std::vector<cv::KeyPoint> Detector::detect(cv::Mat& gray) const {
    int blockSize = 2;     // blockSize
    int apertureSize = 3;  // ksize (Sobel)
    double k = 0.04;       // k (Harris parameter)

    // Harris Corner Detection
    cv::Mat harrisResponse = cv::Mat::zeros(gray.size(), CV_32FC1);
    cv::cornerHarris(gray, harrisResponse, blockSize, apertureSize, k);

    std::vector<cv::KeyPoint> keypoints;

    // Define a treshold
    double maxVal;
    cv::minMaxLoc(harrisResponse, nullptr, &maxVal);
    double threshold = 0.01 * maxVal; // relative to the max value

    // Convert harrisResponse to keypoints
    for (int y = 0; y < harrisResponse.rows; ++y) {
        for (int x = 0; x < harrisResponse.cols; ++x) {
            if (harrisResponse.at<float>(y, x) > threshold) {
                keypoints.emplace_back(cv::Point2f(x, y), 3.0f);
            }
        }
    }

    return keypoints;
}