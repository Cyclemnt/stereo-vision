#include <opencv2/opencv.hpp>
#include <iostream>

#include "camera/camera.hpp"
#include "features/detector.hpp"

int main(int argc, char** argv) {

    // Camera* cam = new Camera("/dev/video2");
    // cam->openCamera();

    cv::Mat img = cv::imread("../kitti_images/000000.png", cv::IMREAD_GRAYSCALE);
    Detector d;
    std::vector<cv::KeyPoint> kp = d.detect(img);
    for (auto& p : kp) {
        std::cout << p.pt.x << " " << p.pt.y << std::endl;
    }

    return 0;
}
