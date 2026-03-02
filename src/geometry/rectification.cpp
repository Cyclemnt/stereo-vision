#include "geometry/rectification.hpp"

StereoRectifier::StereoRectifier() {}

void StereoRectifier::init() {
    cv::Mat K1, K2, R, T;
    // cv::eigen2cv(eK1, K1);
    // cv::eigen2cv(eK2, K2);
    // cv::eigen2cv(eR, R);
    // cv::eigen2cv(eT, T);

    // Compute rectification matrixes
    cv::Mat R1, R2, P1, P2, Q;
    // stereoRectify aligne les caméras sur un plan commun
    // cv::stereoRectify(K1, D1, K2, D2, imageSize, R, T, R1, R2, P1, P2, Q);

    // Generate lookup tables
    // CV_16SC2 (fixed-point) for fast cv::remap
    // cv::initUndistortRectifyMap(K1, D1, R1, P1, imageSize, CV_16SC2, mapL1, mapL2);
    // cv::initUndistortRectifyMap(K2, D2, R2, P2, imageSize, CV_16SC2, mapR1, mapR2);
}

void StereoRectifier::process(const cv::Mat& imgL, const cv::Mat& imgR, cv::Mat& outL, cv::Mat& outR) {
    cv::remap(imgL, outL, mapL1, mapL2, cv::INTER_LINEAR);
    cv::remap(imgR, outR, mapR1, mapR2, cv::INTER_LINEAR);
}