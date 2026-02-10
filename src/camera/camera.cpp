#include "camera/camera.hpp"

#include <iostream>

int Camera::openCamera() {
    std::cout << "opening camera" << std::endl;
    cv::VideoCapture camera(this->cameraUri, cv::CAP_V4L2); // in linux check $ ls /dev/video*, and check what camera to pick using ffplay /dev/videoX (install ffmpeg)
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    // create a window to display the images from the webcam
    cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

    // array to hold image
    cv::Mat frame;

    // display the frame until you press a key
    while (1) {
        // capture the next frame from the webcam
        camera >> frame;
        // show the image on the window
        cv::imshow("Webcam", frame);
        // wait (10ms) for esc key to be pressed to stop
        if (cv::waitKey(10) == 27)
            break;
    }
    return 0;
}