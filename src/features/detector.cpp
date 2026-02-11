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
    double threshold = 0.01 * maxVal; // Relative to the max value

    // Parcours en évitant les bords
    for (int y = 1; y < harrisResponse.rows - 1; ++y) {
        for (int x = 1; x < harrisResponse.cols - 1; ++x) {
            float val = harrisResponse.at<float>(y, x);

            if (val > threshold) {
                bool isLocalMax = true;

                // Watch neighbors in 3x3 window (NMS)
                for (int dy = -1; dy <= 1 && isLocalMax; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dy == 0 && dx == 0) continue;

                        if (harrisResponse.at<float>(y + dy, x + dx) >= val) {
                            isLocalMax = false;
                            break;
                        }
                    }
                }

                // Create a keypoint
                if (isLocalMax) {
                    keypoints.emplace_back(cv::Point2f(x, y), 3.0f, -1, val);
                }
            }
        }
    }

    return keypoints;

}