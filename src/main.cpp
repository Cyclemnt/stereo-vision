#include <opencv2/opencv.hpp>
#include <iostream>

#include "camera/camera.hpp"
#include "features/detector.hpp"
#include "features/descriptor.hpp"
#include "features/matcher.hpp"

int main(int argc, char** argv) {

    // Camera* cam = new Camera("/dev/video2");
    // cam->openCamera();

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

    return 0;
}
