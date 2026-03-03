#ifndef __MATCHER__
#define __MATCHER__

#include <vector>
#include <opencv2/opencv.hpp>

class StereoMatcher {
private:
    float yThreshold = 2.0f; // Tolérance en pixels sur l'alignement Y
    float ratioTest = 0.75f; // Test de Lowe pour la robustesse

public:
    StereoMatcher(float yTol = 2.0f, float ratio = 0.75f);

    /// @brief Match points en utilisant la contrainte épipolaire horizontale
    std::vector<cv::DMatch> match(
        const std::vector<cv::KeyPoint>& kptsL, const cv::Mat& descL,
        const std::vector<cv::KeyPoint>& kptsR, const cv::Mat& descR);

    /// @brief Visualize matches between two images
    /// @param img1 First image
    /// @param kp1 Keypoints from image 1
    /// @param img2 Second image
    /// @param kp2 Keypoints from image 2
    /// @param matches Matches between descriptors
    void visualize(const cv::Mat& img1, const std::vector<cv::KeyPoint>& kp1, const cv::Mat& img2, const std::vector<cv::KeyPoint>& kp2, const std::vector<cv::DMatch>& matches) const;
};

#endif