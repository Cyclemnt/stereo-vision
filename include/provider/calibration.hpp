#ifndef __CALIBRATION__
#define __CALIBRATION__

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

class Calibrator {
private:
    // Chessboard parameters
    cv::Size boardSize = {7, 5}; // corners
    float squareSize = 30.5f;  // mm

    // Accumulated data
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imgPointsL, imgPointsR;
    cv::Size imageSize;

    // Folder paths
    std::string pathL;
    std::string pathR;

public:
    Calibrator(std::string path, cv::Size board = {7, 5}, float size = 30.5f);
    
    /// @brief Load images and detect corners
    /// @return true if enough pairs detected
    /// @param show To show each corner detection
    bool extractCorners(bool show);

    /// @brief Computes matrixes
    void compute(cv::Mat& K1, cv::Mat& D1, cv::Mat& K2, cv::Mat& D2, cv::Mat& R, cv::Mat& T, cv::Size& imgSize);
};

#endif