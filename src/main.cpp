#include <iostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "camera/stereoCamera.hpp"
#include "features/featurePipeline.hpp"

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

void featureDetectorTest() {
    // Feature Detector
    cv::Mat img1 = cv::imread("../kitti_images/000000.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread("../kitti_images/000001.png", cv::IMREAD_GRAYSCALE);

    FeaturePipeline f;

    auto points = f.getMatches(img1, img2);

    // // To go further:
    // // Clean points with RANSAC
    // std::vector<uchar> mask;
    // cv::findFundamentalMat(points.first, points.second, cv::FM_RANSAC, 3.0, 0.99, mask);
    // std::vector<cv::Point2f> cleanPoints1, cleanPoints2;
    // for(int i = 0; i < mask.size(); ++i) {
    //     if(mask[i]) {
    //         cleanPoints1.push_back(points.first[i]);
    //         cleanPoints2.push_back(points.second[i]);
    //     }
    // }

    // // Create P matrixes
    // // We get K1, K2, R and t from camera calibration
    // // P1 = K1 * [I | 0]
    // cv::Mat P1 = cv::Mat::zeros(3, 4, CV_64F);
    // K1.copyTo(P1(cv::Rect(0, 0, 3, 3)));
    // // P2 = K2 * [R | T]
    // cv::Mat P2 = cv::Mat::zeros(3, 4, CV_64F);
    // cv::Mat RT;
    // cv::hconcat(R, T, RT); // RT = R|t
    // P2 = K2 * RT;

    // // Triangulate points
    // cv::Mat points4D;
    // cv::triangulatePoints(P1, P2, cleanPoints1, cleanPoints2, points4D);
    // std::vector<cv::Point3f> pointCloud;
    // for (int i = 0; i < points4D.cols; ++i) {
    //     cv::Mat col = points4D.col(i);
    //     float w = col.at<float>(3, 0);
    //     if (w != 0) {
    //         pointCloud.emplace_back(
    //             col.at<float>(0, 0) / w,
    //             col.at<float>(1, 0) / w,
    //             col.at<float>(2, 0) / w
    //         );
    //     }
    // }
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

int main(int argc, char** argv) {

    featureDetectorTest();

    // stereoCamTest();

    // jsonTest();

    return 0;
}
