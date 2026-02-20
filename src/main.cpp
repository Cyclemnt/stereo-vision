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

    auto pts = f.getMatches(img1, img2);

    for (auto& pt : pts) {
        std::cout << pt.first.x << "," << pt.first.y << " <=> " << pt.second.x << "," << pt.second.y << std::endl;
    }
    std::cout << "total matches: " << pts.size() << std::endl;
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
