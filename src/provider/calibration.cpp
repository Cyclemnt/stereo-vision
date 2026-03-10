#include "provider/calibration.hpp"
#include <iostream>

Calibrator::Calibrator(std::string path, cv::Size board, float size) 
    : boardSize(board), squareSize(size), pathL(path+"left/"), pathR(path+"right/") {}

bool Calibrator::extractCorners(bool show) {
    std::vector<std::string> fnL, fnR;
    cv::glob(pathL + "*.png", fnL);
    cv::glob(pathR + "*.png", fnR);
    std::sort(fnL.begin(), fnL.end());
    std::sort(fnR.begin(), fnR.end());

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

            // magic: verify orientation
            auto fixOrientation = [](std::vector<cv::Point2f>& pts, int i) {
                // compute distance from (0,0) for first and last corner
                float dFirst = pts.front().x * pts.front().x + pts.front().y * pts.front().y;
                float dLast = pts.back().x * pts.back().x + pts.back().y * pts.back().y;
                if (dFirst > dLast) {
                    std::cout << "[!] inversion correction on pair " << i << std::endl;
                    std::reverse(pts.begin(), pts.end());
                }
            };

            fixOrientation(cornersL, i);
            fixOrientation(cornersR, i);

            // vizualization
            if (show) {
                cv::Mat viewLR;
                cv::Mat viewL = imgL.clone();
                cv::Mat viewR = imgR.clone();
                cv::drawChessboardCorners(viewL, boardSize, cornersL, foundL);
                cv::drawChessboardCorners(viewR, boardSize, cornersR, foundR);
                cv::hconcat(viewL, viewR, viewLR);
                cv::imshow("Chessboard corners", viewLR);
                cv::waitKey();
            }

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

void Calibrator::compute(cv::Mat& K1, cv::Mat& D1, cv::Mat& K2, cv::Mat& D2, cv::Mat& R, cv::Mat& T, cv::Size& imgSize) {
    if (objectPoints.size() < 10) std::cerr << "Warning: Not enough valid pairs for good calibration." << std::endl;

    cv::Mat E, F;
    std::vector<cv::Mat> rvecs, tvecs;
    imgSize = this->imageSize;

    // Mono calibrations
    std::cout << "Computing intrinsics matrixes..." << std::endl;
    double rms1 = cv::calibrateCamera(objectPoints, imgPointsL, imageSize, K1, D1, rvecs, tvecs);
    double rms2 = cv::calibrateCamera(objectPoints, imgPointsR, imageSize, K2, D2, rvecs, tvecs);
    std::cout << "RMS1 error: " << rms1 << std::endl;
    std::cout << "RMS2 error: " << rms2 << std::endl;
    
    // Stereo calibration
    std::cout << "Computing stereo calibration..." << std::endl;
    int flags = cv::CALIB_USE_INTRINSIC_GUESS;
    double rms = cv::stereoCalibrate(objectPoints, imgPointsL, imgPointsR, K1, D1, K2, D2, imageSize, R, T, E, F, flags, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-6));
    std::cout << "RMS error: " << rms << std::endl;    
    std::cout << "Calibration is done" << std::endl;
}
