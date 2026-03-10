#include "features/detector.hpp"
#include <limits>

Detector::Detector() {}

std::vector<cv::KeyPoint> Detector::detect(cv::Mat& gray) const {
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();

    std::vector<cv::KeyPoint> keypoints;

    detector->detect(gray, keypoints);

    return keypoints;

    // int blockSize = 2;     // blockSize
    // int apertureSize = 3;  // ksize (Sobel)
    // double k = 0.04;       // k (Harris parameter)

    // // Harris Corner Detection
    // cv::Mat harrisResponse = cv::Mat::zeros(gray.size(), CV_32FC1);
    // cv::cornerHarris(gray, harrisResponse, blockSize, apertureSize, k);

    // // Compute angle of each px
    // cv::Mat grad_x, grad_y, magnitude, angle;
    // cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    // cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);
    // cv::phase(grad_x, grad_y, angle, true);

    // std::vector<cv::KeyPoint> keypoints;

    // // Define a treshold
    // double maxVal = std::numeric_limits<double>::max();
    // cv::minMaxLoc(harrisResponse, nullptr, &maxVal);
    // double threshold = 0.00001 * maxVal; // Relative to the max value
    // int nmsWindowSize = 3;
    // int halfWindow = (nmsWindowSize - 1) * 0.5;
    // size_t maxPoints = 20000;

    // // Read harrisResponse avoiding borders
    // for (int y = 1; y < harrisResponse.rows - 1; ++y) {
    //     for (int x = 1; x < harrisResponse.cols - 1; ++x) {
    //         float val = harrisResponse.at<float>(y, x);

    //         if (val > threshold) {
    //             bool isLocalMax = true;

    //             // Watch neighbors in the defined window (NMS)
    //             for (int dy = -halfWindow; dy <= halfWindow && isLocalMax; ++dy) {
    //                 for (int dx = -halfWindow; dx <= halfWindow; ++dx) {
    //                     if (dy == 0 && dx == 0) continue;

    //                     if (harrisResponse.at<float>(y + dy, x + dx) >= val) {
    //                         isLocalMax = false;
    //                         break;
    //                     }
    //                 }
    //             }

    //             // Create a keypoint
    //             if (isLocalMax) {
    //                 float a = angle.at<float>(y, x);
    //                 keypoints.emplace_back(cv::Point2f(x, y), halfWindow, a, val);
    //             }
    //         }
    //     }
    // }

    // if (keypoints.size() > (size_t)maxPoints) {
    //     std::sort(keypoints.begin(), keypoints.end(), [](const cv::KeyPoint& a, const cv::KeyPoint& b) {
    //         return a.response > b.response;
    //     });
    //     keypoints.erase(keypoints.begin() + maxPoints, keypoints.end());
    // }

    // return keypoints;
}

void Detector::visualize(const cv::Mat& image, const std::vector<cv::KeyPoint>& keypoints) const {
    cv::Mat output;

    // If input is grayscale, convert to BGR to draw with colors
    if (image.channels() == 1) {
        cv::cvtColor(image, output, cv::COLOR_GRAY2BGR);
    } else {
        output = image.clone();
    }

    // Draw keypoints in red
    cv::drawKeypoints(output, keypoints, output, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imshow("Corners", output);
    cv::waitKey(0);
}
