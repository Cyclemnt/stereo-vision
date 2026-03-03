#include "geometry/rectification.hpp"
#include <opencv2/core/eigen.hpp>

StereoRectifier::StereoRectifier(const Eigen::Matrix3d& K1, const cv::Mat& D1, const Eigen::Matrix3d& K2, const cv::Mat& D2, const Eigen::Matrix3d& R,  const Eigen::Vector3d& T, cv::Size imgSize) {
    // Conversion from Eigen to OpenCV
    cv::Mat cvK1, cvK2, cvR, cvT;
    cv::eigen2cv(K1, cvK1);
    cv::eigen2cv(K2, cvK2);
    cv::eigen2cv(R, cvR);
    cv::eigen2cv(T, cvT);

    // Compute rectification matrixes
    cv::Mat R1, R2, P1, P2, Q;
    // stereoRectify aligne les caméras sur un plan commun
    cv::stereoRectify(cvK1, D1, cvK2, D2, imgSize, cvR, cvT, R1, R2, P1, P2, Q);

    // Generate lookup tables
    // CV_16SC2 (fixed-point) for fast cv::remap
    cv::initUndistortRectifyMap(cvK1, D1, R1, P1, imgSize, CV_16SC2, mapL1, mapL2);
    cv::initUndistortRectifyMap(cvK2, D2, R2, P2, imgSize, CV_16SC2, mapR1, mapR2);
}

void StereoRectifier::process(const cv::Mat& imgL, const cv::Mat& imgR, cv::Mat& outL, cv::Mat& outR) {
    cv::remap(imgL, outL, mapL1, mapL2, cv::INTER_LINEAR);
    cv::remap(imgR, outR, mapR1, mapR2, cv::INTER_LINEAR);
}