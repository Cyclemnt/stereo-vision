#ifndef __STEREO_RECONSTRUCTION_PIPELINE__
#define __STEREO_RECONSTRUCTION_PIPELINE__

#include <memory>
#include <string>
#include "provider/stereoSystem.hpp"

class StereoReconstructionPipeline {
private:
    std::unique_ptr<StereoSystem<Camera>> camera;

public:
    StereoReconstructionPipeline(std::string leftCamName, std::string rightCamName);

    ~StereoReconstructionPipeline() = default;
    // 3DPointCloud run();
};

#endif