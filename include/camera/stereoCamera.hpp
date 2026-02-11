#ifndef __STEREO_CAMERA__
#define __STEREO_CAMERA__

#include <memory>
class Camera;

class StereoCamera {
private:
    std::unique_ptr<Camera> leftCam;
    std::unique_ptr<Camera> rightCam;

public:
    /// @brief Constructs two cameras
    StereoCamera(/* params */);

    Camera& left() noexcept;
    Camera& right() noexcept;

    const Camera& left() const noexcept;
    const Camera& right() const noexcept;

};

#endif