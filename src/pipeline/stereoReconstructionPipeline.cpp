#include "pipeline/stereoReconstructionPipeline.hpp"
#include "camera/stereoCamera.hpp"

StereoReconstructionPipeline::StereoReconstructionPipeline(std::string leftCamName, std::string rightCamName) {
    camera = std::make_unique<StereoCamera>(leftCamName, rightCamName);
}

// 3DPointCloud run() {}