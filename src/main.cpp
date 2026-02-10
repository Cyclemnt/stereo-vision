#include <opencv2/opencv.hpp>
#include <iostream>

#include "camera/camera.hpp"

int main(int argc, char** argv) {

    Camera* cam = new Camera("/dev/video2");
    cam->openCamera();

    return 0;
}
