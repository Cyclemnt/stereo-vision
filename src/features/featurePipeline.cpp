#include "features/featurePipeline.hpp"
#include "features/detector.hpp"
#include "features/descriptor.hpp"
#include "features/matcher.hpp"

FeaturePipeline::FeaturePipeline()
    : detector(std::make_unique<Detector>()), descriptor(std::make_unique<Descriptor>()), matcher(std::make_unique<Matcher>()) {}

FeaturePipeline::~FeaturePipeline() = default;

std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> FeaturePipeline::getMatches(cv::Mat& img1, cv::Mat& img2) {

    std::vector<cv::KeyPoint> keypoints1 = detector->detect(img1);
    std::vector<cv::KeyPoint> keypoints2 = detector->detect(img2);

    cv::Mat descriptors1 = descriptor->compute(img1, keypoints1);
    cv::Mat descriptors2 = descriptor->compute(img2, keypoints2);
    
    std::vector<cv::DMatch> matches = matcher->match(descriptors1, descriptors2);


    std::vector<cv::Point2f> points1, points2;

    for (const auto& m : matches) {
        points1.push_back(keypoints1[m.queryIdx].pt);
        points2.push_back(keypoints2[m.trainIdx].pt);
    }

    return std::make_pair(points1, points2);
}