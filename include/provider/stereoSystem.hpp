#ifndef __STEREO_CAMERA__
#define __STEREO_CAMERA__

#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "provider/camera.hpp"
#include "provider/image.hpp"

/*
StereoCamera manages two threads, corresponding to the two cameras aquisitions
*/

template <typename T>
concept ImageProvider = std::derived_from<T, ImageSource>;

template <ImageProvider Provider>
class StereoSystem
{
private:
    std::atomic<bool> isCapturing;
    std::mutex leftCamMtx;
    std::mutex rightCamMtx;
    std::thread leftT;
    std::thread rightT;

    std::unique_ptr<Provider> leftDev;
    std::unique_ptr<Provider> rightDev;

public:
    // This constructor allow for
    // - Create both cameras
    // - Tweak each camera's settings / check if functionning well
    // - Create the StereoCamera object, which takes full control of the cameras
    StereoSystem(std::unique_ptr<Provider> left, std::unique_ptr<Provider> right)
    {
        // Takes ownership of both cameras
        this->leftDev = std::move(left);
        this->rightDev = std::move(right);
    }

    StereoSystem(std::string leftCameraName, std::string rightCameraName); // The two cameras might not be the same

    ~StereoSystem();

    // Having access to cameras when they are already rolling syncronized would disrupt everything
    // So we first get the two cameras working independantly
    // Then save them, its settings, uri...
    // And then load them directly here with the constructor
    // Camera& left() noexcept;
    // Camera& right() noexcept;

    const Provider &leftDevice() const noexcept
    {
        return *this->leftDev;
    }
    const Provider &rightDevice() const noexcept
    {
        return *this->rightDev;
    }

    /// @brief Open the Left camera from the Stereo system (to check integration)
    void openLeftCameraFeed();

    /// @brief Open the Right camera from the Stereo system (to check integration)
    void openRightCameraFeed();

    /// @brief Open both cameras at the same time in two different windows to see syncronization
    void openCameraFeeds();

    /// @brief Start the syncronized aquisition. Be aware that the first captures might take 500ms, for syncronization's sake
    void start(); // Make both cameras take a simultaneous image

    /// @brief Kindly ask the multi-threading god to stop this non-sense (and stop the syncronized capture from both cameras)
    void stop(); // Make both cameras take a simultaneous image

    /// @brief Get a matching pair of images from both cameras
    /// @return Pair (left, right) of the camera images
    std::pair<cv::Mat, cv::Mat> capturePair();
};

#endif