#include "camera/stereoCamera.hpp"
#include "camera/camera.hpp"

StereoCamera::StereoCamera(std::string leftCameraName, std::string rightCameraName) {
    leftCam = std::make_unique<Camera>(leftCameraName);
    rightCam = std::make_unique<Camera>(rightCameraName);
}

Camera& StereoCamera::left() noexcept {
    return *leftCam;
}

Camera& StereoCamera::right() noexcept {
    return *rightCam;
}

const Camera& StereoCamera::left() const noexcept {
    return *leftCam;
}

const Camera& StereoCamera::right() const noexcept {
    return *rightCam;
}

void StereoCamera::capture() {
    // Crucial metric : both pictures should be taken at the EXACT SAME time
    // Instead of using the default take Picture, we dismantle the two steps (grab, retrieve) in two steps
    leftCam->takePicture();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    rightCam->takePicture();
}