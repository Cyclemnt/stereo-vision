#ifndef __IMAGE_PROVIDER__
#define __IMAGE_PROVIDER__

#include <iostream>
#include <concepts>
#include <type_traits>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "files/fileManager.hpp"

/*
    This class is an abstract class, that managed both Physical Cameras and
   Images Images are great for prototyping, once data is captured once And
   Cameras are great for after, to have the online recontruction pipeline
*/

using K_datatype = double; // Need to be centralized, as the type is needed for JSON files (serialization)

class ImageSource
{
protected:
    mutable cv::Mat frame = cv::Mat::zeros(1, 1, CV_8UC3); // For parallelization, the image is on the heap (OpenCV default hidden behavior)
    std::string deviceSavedName = "unknownDevice";         // Used to save camera presets, makes working with multiple cameras easier

    // Intrinsic paramaters
    // Must be set BEFORE creating the StereoCamera object
    double focalLength = std::numeric_limits<double>::max();
    double skew = std::numeric_limits<double>::max();
    std::pair<double, double> principalPoint = std::make_pair(
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max());
    int fps = 0; // Do not support decimal fps
    std::pair<int, int> definitionPixels = std::make_pair(0, 0);

    Eigen::Matrix<K_datatype, 3, 3> K = Eigen::Matrix<K_datatype, 3, 3>::Zero(); // Calibration matrix {f_x s 0 \ 0 f_y 0 \ 0 0 1}
    // f_x = focalLength * definitionPixels.first
    // f_x = focalLength * definitionPixels.second

    ImageSource() = default;

public:
    ~ImageSource() = default;

    virtual void takePicture() = 0;
    virtual const cv::Mat getPicture() const = 0;
    virtual bool save(std::string path) const = 0;

    /// @brief Little usefull macro to open an image in another window
    /// @param img OpenCV image
    void static showPicture(const cv::Mat &img);
};

#endif