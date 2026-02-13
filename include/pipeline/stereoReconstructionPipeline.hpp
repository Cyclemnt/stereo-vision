#ifndef __STEREO_RECONSTRUCTION_PIPELINE__
#define __STEREO_RECONSTRUCTION_PIPELINE__

#include <memory>
#include <string>
class StereoCamera;
#include "camera/stereoCamera.hpp"

class StereoReconstructionPipeline {
private:
    std::unique_ptr<StereoCamera> camera;

public:
    StereoReconstructionPipeline(std::string leftCamName, std::string rightCamName);

    ~StereoReconstructionPipeline() = default;
    // 3DPointCloud run();
};

#endif