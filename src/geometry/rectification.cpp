#include "geometry/rectification.hpp"
#include <opencv2/core/eigen.hpp>
#include <iostream>

StereoRectifier::StereoRectifier(const cv::Mat& K1, const cv::Mat& D1, const cv::Mat& K2, const cv::Mat& D2, const cv::Mat& R,  const cv::Mat& T, cv::Size imgSize) {
    // Compute rectification matrixes
    cv::Mat R1, R2, P1, P2, Q;
    // stereoRectify aligne les caméras sur un plan commun
    cv::stereoRectify(K1, D1, K2, D2, imgSize, R, T, R1, R2, P1, P2, Q);

    // Generate lookup tables
    // CV_16SC2 (fixed-point) for fast cv::remap
    cv::initUndistortRectifyMap(K1, D1, R1, P1, imgSize, CV_16SC2, mapL1, mapL2);
    cv::initUndistortRectifyMap(K2, D2, R2, P2, imgSize, CV_16SC2, mapR1, mapR2);

    std::cout << "Rectification is done" << std::endl;
}

void StereoRectifier::process(const cv::Mat& imgL, const cv::Mat& imgR, cv::Mat& outL, cv::Mat& outR) {
    cv::remap(imgL, outL, mapL1, mapL2, cv::INTER_LINEAR);
    cv::remap(imgR, outR, mapR1, mapR2, cv::INTER_LINEAR);
}

void StereoRectifier::vizualize(const cv::Mat& rectL, const cv::Mat& rectR) {
    cv::Mat canvas;
    cv::hconcat(rectL, rectR, canvas);

    // line every 25 px
    for (int y = 0; y < canvas.rows; y += 25) {
        cv::line(canvas, cv::Point(0, y), cv::Point(canvas.cols, y), cv::Scalar(0, 255, 0), 1);
    }

    cv::imshow("Rectification", canvas);
    cv::waitKey(0);
}