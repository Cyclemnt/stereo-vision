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
