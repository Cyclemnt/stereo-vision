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
    int nmsWindowSize = 3;
    int halfWindow = (nmsWindowSize - 1) * 0.5;

    // Read harrisResponse avoiding borders
    for (int y = 1; y < harrisResponse.rows - 1; ++y) {
        for (int x = 1; x < harrisResponse.cols - 1; ++x) {
            float val = harrisResponse.at<float>(y, x);

            if (val > threshold) {
                bool isLocalMax = true;

                // Watch neighbors in the defined window (NMS)
                for (int dy = -halfWindow; dy <= halfWindow && isLocalMax; ++dy) {
                    for (int dx = -halfWindow; dx <= halfWindow; ++dx) {
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

cv::Mat Detector::visualize(const cv::Mat& image, const std::vector<cv::KeyPoint>& keypoints) const {
    cv::Mat output;

    // If input is grayscale, convert to BGR to draw with colors
    if (image.channels() == 1) {
        cv::cvtColor(image, output, cv::COLOR_GRAY2BGR);
    } else {
        output = image.clone();
    }

    // Draw keypoints in red
    cv::drawKeypoints(output, keypoints, output, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    return output;
}
