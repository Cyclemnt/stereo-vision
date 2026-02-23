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
    std::string pathL = "calibration/left/";
    std::string pathR = "calibration/right/";

public:
    Calibrator(cv::Size board = {7, 5}, float size = 30.5f);
    
    /// @brief Load images and detect corners
    /// @return true if enough pairs detected
    bool extractCorners();

    /// @brief Computes matrixes and export to yaml
    void computeAndSave(const std::string& outputYaml);
};

#endif