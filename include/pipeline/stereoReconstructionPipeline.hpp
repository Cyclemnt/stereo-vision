#ifndef __STEREO_RECONSTRUCTION_PIPELINE__
#define __STEREO_RECONSTRUCTION_PIPELINE__

#include <memory>
class StereoCamera;

class StereoReconstructionPipeline {
private:
    std::unique_ptr<StereoCamera> camera;

public:
    StereoReconstructionPipeline(/* args */);
    // 3DPointCloud run();
};

#endif