#include "camera/calibration.hpp"
#include <iostream>

Calibrator::Calibrator(cv::Size board, float size) 
    : boardSize(board), squareSize(size) {}

bool Calibrator::extractCorners() {
    std::vector<std::string> fnL, fnR;
    cv::glob(pathL + "*.jpg", fnL);
    cv::glob(pathR + "*.jpg", fnR);

    if (fnL.size() != fnR.size() || fnL.empty()) {
        std::cerr << "Error: Different number of images or empty folder." << std::endl;
        return false;
    }

    std::cout << "Analyzing " << fnL.size() << " image pairs..." << std::endl;

    for (size_t i = 0; i < fnL.size(); i++) {
        cv::Mat imgL = cv::imread(fnL[i]);
        cv::Mat imgR = cv::imread(fnR[i]);
        
        if (imgL.empty() || imgR.empty()) continue;
        imageSize = imgL.size();

        cv::Mat grayL, grayR;
        cv::cvtColor(imgL, grayL, cv::COLOR_BGR2GRAY);
        cv::cvtColor(imgR, grayR, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> cornersL, cornersR;
        
        // Corner detection
        bool foundL = cv::findChessboardCorners(grayL, boardSize, cornersL, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
        bool foundR = cv::findChessboardCorners(grayR, boardSize, cornersR, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

        if (foundL && foundR) {
            // Refinement
            cv::cornerSubPix(grayL, cornersL, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));
            cv::cornerSubPix(grayR, cornersR, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));

            imgPointsL.push_back(cornersL);
            imgPointsR.push_back(cornersR);
            
            // Generate 3D points (Z = 0)
            std::vector<cv::Point3f> obj;
            for(int h=0; h<boardSize.height; ++h)
                for(int w=0; w<boardSize.width; ++w)
                    obj.push_back(cv::Point3f(w * squareSize, h * squareSize, 0.0f));
            objectPoints.push_back(obj);

            std::cout << "[+] Pair " << i << " valid." << std::endl;
        } else {
            std::cout << "[-] Pair " << i << " rejected." << std::endl;
        }
    }
    return !objectPoints.empty();
}

void Calibrator::computeAndSave(const std::string& outputYaml) {
    if (objectPoints.size() < 10) {
        std::cerr << "Warning: Not enough valid pairs for good calibration." << std::endl;
    }

    cv::Mat K1, D1, K2, D2, R, T, E, F;
    
    std::cout << "Computing stereo calibration..." << std::endl;

    // 1. Stereo calibration
    double rms = cv::stereoCalibrate(objectPoints, imgPointsL, imgPointsR,
                    K1, D1, K2, D2, imageSize, R, T, E, F,
                    cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_SAME_FOCAL_LENGTH,
                    cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-6));

    std::cout << "Success! RMS error: " << rms << std::endl;

    // 2. Rectification (to obtain projection matrices P1, P2)
    cv::Mat R1, R2, P1, P2, Q;
    cv::stereoRectify(K1, D1, K2, D2, imageSize, R, T, R1, R2, P1, P2, Q);

    // 3. Save
    cv::FileStorage fs(outputYaml, cv::FileStorage::WRITE);
    fs << "K1" << K1 << "D1" << D1;
    fs << "K2" << K2 << "D2" << D2;
    fs << "R" << R << "T" << T;
    fs << "P1" << P1 << "P2" << P2 << "Q" << Q;
    fs.release();

    std::cout << "Matrices saved to: " << outputYaml << std::endl;
}