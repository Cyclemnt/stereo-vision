#ifndef __CAMERA__
#define __CAMERA__

#include <tuple>
#include <string>
#include <opencv2/opencv.hpp> 

class Camera {
private:
    std::string cameraSavedName; // Used to save camera presets, makes working with multiple cameras easier
    std::string cameraUri;
    double focalLength;

    std::pair<int, int> definitionPixels;


public:
    // Use this when using a new camera 
    Camera(std::string cameraUri, std::pair<int, int> definition, double focalLength);

    bool setCameraName(std::string newName) {
        cameraSavedName = newName;

        StereoCamera* cam = new Stereo("fbu");
        cam->left->getLocalLength
    }


    Camera(std::string cameraSavedName) {
        data = loadFile(ENUM::savedCamera, cameraSavedName);
        data.focalLength
            data.definitionPixels_x
            data.definitionPixels_y
    };
    // Camera(std::string cameraUri) : cameraUri(cameraUri) {};

    ~Camera();
    int openCamera();
};



#endif