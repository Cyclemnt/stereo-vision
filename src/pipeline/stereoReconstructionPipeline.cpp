#include "pipeline/stereoReconstructionPipeline.hpp"

StereoReconstructionPipeline::StereoReconstructionPipeline(std::string leftCamName, std::string rightCamName)
{
    camera = std::make_unique<StereoSystem<Camera>>(leftCamName, rightCamName);
}

// 3DPointCloud run() {}