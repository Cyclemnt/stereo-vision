#include <iostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "camera/stereoCamera.hpp"
#include "features/featurePipeline.hpp"

#include "camera/calibration.hpp"
#include "geometry/rectification.hpp"
#include "features/detector.hpp"
#include "features/descriptor.hpp"
#include "features/matcher.hpp"

#include "files/fileManager.hpp"
#include <fstream> // Needed for jsonTest(). Should be removed

void availableCamTest() {
    std::vector<std::string> allCams = Camera::availableCameras();
    std::cout << "Available cameras on this computer : \n";
    for (const std::string& camPath : allCams)
        std::cout << camPath << '\n';
    std::cout << std::endl;
}

void stereoCamTest() {
    StereoCamera* stereo;
    {
        // Scope to limit access to leftCamera and rightCamera (they become nullptr after the std::move)
        std::unique_ptr<Camera> leftCamera = std::make_unique<Camera>("Left");
        std::unique_ptr<Camera> rightCamera = std::make_unique<Camera>("Right");

        leftCamera->setUri("/dev/video4");
        rightCamera->setUri("/dev/video2");

        leftCamera->setupCamera();
        rightCamera->setupCamera();

        stereo = new StereoCamera(std::move(leftCamera), std::move(rightCamera));
    }
    // std::cout << stereo->left() << "\n";
    // std::cout << stereo->right() << std::endl;

    // stereo->openLeftCameraFeed();
    // stereo->openRightCameraFeed();
    stereo->openCameraFeeds();
    // stereo->start();

    // cv::Mat leftPic = stereo->left().getPicture();
    // std::cout << "Left pic empty : " << leftPic.empty() << std::endl;
    // cv::Mat rightPic = stereo->right().getPicture();
    // std::cout << "Right pic empty : " << rightPic.empty() << std::endl;

    // Camera::showPicture(leftPic);
    // Camera::showPicture(rightPic);

    // std::this_thread::sleep_for(std::chrono::milliseconds(750));

    delete stereo;

    std::cout << "deteled stereo" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void jsonTest() {
    json data;
    Files::ERRORS status;

    // Import
    status = Files::loadCameraFile("Logitech (CO-ROB-398-CAM)", &data);
    if (status == Files::ERRORS::OK) {
        std::cout << "It worked, here is the json parsed : " << data.dump(4) << std::endl;
        Camera cam = data;
        std::cout << cam << std::endl;
    }
    else {
        std::cout << Files::to_string(status) << std::endl;
    }

    // json j;
    // j["name"] = "Lucas";
    // j["age"] = "21";
    // j["parents"] = {
    //     {"father", "Sébastien"},
    //     {"mother", "Rosine"}
    // };

    Camera left = Camera();
    left.setUri("/dev/video42");
    left.setName("Logitech (CO-ROB-398-CAM)");

    json j = left;

    std::cout << j.dump(3) << std::endl;

    // Writing json file
    status = Files::saveCameraFile("Logitech (CO-ROB-398-CAM)", &j);
    if (status == Files::ERRORS::OK) {
        std::cout << "Camera save worked !" << std::endl;
    }
    else {
        std::cout << Files::to_string(status) << std::endl;
    }

}


cv::Mat computeSparseDisparity(const std::vector<cv::KeyPoint>& kptsL, const std::vector<cv::KeyPoint>& kptsR, const std::vector<cv::DMatch>& matches, cv::Size imgSize) {
    cv::Mat disparity = cv::Mat::zeros(imgSize, CV_32F);

    for (const auto& m : matches) {
        const auto& kpL = kptsL[m.queryIdx];
        const auto& kpR = kptsR[m.trainIdx];

        int x = (int)kpL.pt.x;
        int y = (int)kpL.pt.y;

        if (x < 0 || x >= imgSize.width || y < 0 || y >= imgSize.height) continue;

        float disp = kpL.pt.x - kpR.pt.x;
        if (disp > 0) disparity.at<float>(y, x) = disp;
    }

    return disparity;
}

void showDisparity(const cv::Mat& disparity) {
    cv::Mat dispVis;
    cv::normalize(disparity, dispVis, 0, 255, cv::NORM_MINMAX);
    dispVis.convertTo(dispVis, CV_8U);

    cv::applyColorMap(dispVis, dispVis, cv::COLORMAP_JET);

    cv::imshow("Disparity", dispVis);
    cv::waitKey(0);
}

void theWholeProcess() {
    // Calibrate
    Calibrator calib("../calibration/robotnik/", {5, 7}, 30.5f);
    cv::Mat K1, D1, K2, D2, R, T;
    cv::Size imgSize;
    if (calib.extractCorners(false)) { // change to true to see each detection
        calib.compute(K1, D1, K2, D2, R, T, imgSize);
    }

    // Read
    cv::Mat imgL = cv::imread("../calibration/robotnik/pic/left/l103.png");
    cv::Mat imgR = cv::imread("../calibration/robotnik/pic/right/r103.png");

    // Rectify
    StereoRectifier rectifier(K1, D1, K2, D2, R, T, imgSize);
    cv::Mat rectifiedL, rectifiedR;
    rectifier.process(imgL, imgR, rectifiedL, rectifiedR);
    rectifier.vizualize(rectifiedL, rectifiedR);

    // Convert to grayscale
    cv::Mat grayL, grayR;
    cv::cvtColor(rectifiedL, grayL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rectifiedR, grayR, cv::COLOR_BGR2GRAY);

    // Detect keypoints 
    Detector detector;
    std::vector<cv::KeyPoint> keypoints1 = detector.detect(grayL);
    std::vector<cv::KeyPoint> keypoints2 = detector.detect(grayR);
    std::cout << keypoints1.size() << " kpts on left, " << keypoints2.size() << " kpts on right" << std::endl;
    detector.visualize(grayL, keypoints1);
    detector.visualize(grayR, keypoints2);

    // Descript keypoints
    Descriptor descriptor;
    cv::Mat descriptors1 = descriptor.compute(rectifiedL, keypoints1);
    cv::Mat descriptors2 = descriptor.compute(rectifiedR, keypoints2);

    // Match keypoints
    StereoMatcher matcher;
    std::vector<cv::DMatch> matches = matcher.match(keypoints1, descriptors1, keypoints2, descriptors2);
    std::cout << matches.size() << " matches" << std::endl;
    matcher.visualize(rectifiedL, keypoints1, rectifiedR, keypoints2, matches);

    // Compute sparse disparity map
    cv::Mat disparity = computeSparseDisparity(keypoints1, keypoints2, matches, imgSize);
    showDisparity(disparity);
}

int main(int argc, char** argv) {

    theWholeProcess();

    // stereoCamTest();

    // jsonTest();

    return 0;
}
