#ifndef __STEREO_CAMERA__
#define __STEREO_CAMERA__

#include <memory>
#include <string>

#include "../camera/camera.hpp" // Forward declaration can't be used here, std::unique_ptr needs to know the size of Camera

class StereoCamera {
private:
    std::unique_ptr<Camera> leftCam;
    std::unique_ptr<Camera> rightCam;

public:
    StereoCamera(std::string leftCameraName, std::string rightCameraName); // The two cameras might not be the same

    Camera& left() noexcept;
    Camera& right() noexcept;

    const Camera& left() const noexcept;
    const Camera& right() const noexcept;

};

#endif