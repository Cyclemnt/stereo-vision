#include <opencv2/opencv.hpp>
#include <iostream>

#include "camera/stereoCamera.hpp"
#include "features/detector.hpp"
#include "features/descriptor.hpp"
#include "features/matcher.hpp"

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

int main(int argc, char** argv) {

    // Feature Detector
    cv::Mat img1 = cv::imread("../kitti_images/000000.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread("../kitti_images/000001.png", cv::IMREAD_GRAYSCALE);
    Detector detec;
    Descriptor desc;
    Matcher match;
    std::vector<cv::KeyPoint> keypoints1 = detec.detect(img1);
    std::vector<cv::KeyPoint> keypoints2 = detec.detect(img2);
    cv::Mat descriptors1 = desc.compute(img1, keypoints1);
    cv::Mat descriptors2 = desc.compute(img2, keypoints2);
    std::vector<cv::DMatch> matches = match.match(descriptors1, descriptors2);

    cv::Mat vizkp1 = detec.visualize(img1, keypoints1);
    cv::Mat vizkp2 = detec.visualize(img2, keypoints2);

    cv::Mat vizmatch = match.visualize(img1, keypoints1, img2, keypoints2, matches);

    cv::imshow("Keypoints 1", vizkp1);
    cv::imshow("Keypoints 2", vizkp1);

    cv::imshow("Matches", vizmatch);

    cv::waitKey(0);


    // Stereo Cameras
    // std::vector<std::string> allCams = Camera::availableCameras();
    // for (const std::string& camPath : allCams)
    //     std::cout << camPath << std::endl;

    stereoCamTest();

    return 0;
}
