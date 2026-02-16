#ifndef __STEREO_CAMERA__
#define __STEREO_CAMERA__

#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "../camera/camera.hpp" // Forward declaration can't be used here, std::unique_ptr needs to know the size of Camera

/*
StereoCamera manages two threads, corresponding to the two cameras aquisitions
*/

class StereoCamera {
private:
    std::atomic<bool> isCapturing;
    std::mutex leftCamMtx;
    std::mutex rightCamMtx;
    std::thread leftT;
    std::thread rightT;

    std::unique_ptr<Camera> leftCam;
    std::unique_ptr<Camera> rightCam;

public:
    // This constructor allow for 
    // - Create both cameras
    // - Tweak each camera's settings / check if functionning well
    // - Create the StereoCamera object, which takes full control of the cameras
    StereoCamera(std::unique_ptr<Camera> left, std::unique_ptr<Camera> right) {
        // Takes ownership of both cameras
        this->leftCam = std::move(left);
        this->rightCam = std::move(right);
    }

    StereoCamera(std::string leftCameraName, std::string rightCameraName); // The two cameras might not be the same

    ~StereoCamera();

    // Having access to cameras when they are already rolling syncronized would disrupt everything
    // So we first get the two cameras working independantly
    // Then save them, its settings, uri...
    // And then load them directly here with the constructor
    // Camera& left() noexcept;
    // Camera& right() noexcept;

    const Camera& left() const noexcept;
    const Camera& right() const noexcept;

    void openLeftCameraFeed();
    void openRightCameraFeed();
    void openCameraFeeds();
    void start(); // Make both cameras take a simultaneous image
    void stop(); // Make both cameras take a simultaneous image

    std::pair<cv::Mat, cv::Mat> capturePair();
};

#endif