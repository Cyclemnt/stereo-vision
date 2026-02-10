#include "camera/stereoCamera.hpp"
#include "camera/camera.hpp"

StereoCamera::StereoCamera(/* params */) {
    leftCam = std::make_unique<Camera>(/* params */);
    rightCam = std::make_unique<Camera>(/* params */);
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
