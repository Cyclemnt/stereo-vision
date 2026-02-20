#ifndef __FEATUREPIPELINE__
#define __FEATUREPIPELINE__

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>

class Detector; class Descriptor; class Matcher;

class FeaturePipeline {
private:
    std::unique_ptr<Detector> detector;
    std::unique_ptr<Descriptor> descriptor;
    std::unique_ptr<Matcher> matcher;

public:
    FeaturePipeline();
    ~FeaturePipeline();

    /// @brief Detects features, computes desctiptors and find matches
    /// @param img1 One of the images
    /// @param img2 One of the images
    /// @return Two vectors containing matching points on both images (having same index)
    std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> getMatches(cv::Mat& img1, cv::Mat& img2);
};

#endif // __FEATUREPIPELINE__