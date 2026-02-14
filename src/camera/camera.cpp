#include "camera/camera.hpp"

#include <iostream>
#include <stdexcept>
#include <filesystem>

/*
Great commands for cameras
v4l2-ctl --device=/dev/videoX <options>
options :
    --list-ctrls : shows the parameters of the cameras (brightness, saturation...)
*/

void Camera::setupCamera() {
    // Opens the camera at the given uri
    assert(this->cameraUri != ""); // cameraUri is empty, can't open camera

    this->cam = cv::VideoCapture(this->cameraUri, cv::CAP_V4L2); // in linux check $ ls /dev/video*, and check what camera to pick using ffplay /dev/videoX (install ffmpeg)
    if (!this->cam.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        throw std::runtime_error("Camera not available on " + this->cameraUri);
    }

    // Check with v4l2-ctl --device=/dev/videoX --list-formats-ext
    // std::cout << "Setting cam to YUYV : " << set_ok << "\n";
    if (!this->cam.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'))) {
        // Fallback for some cameras
        // std::cout << "Setting cam to YUY2 : " << set_ok << std::endl;
        if (!this->cam.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', '2'))) {
            throw std::runtime_error("Camera could not be read in YUYV nor YUY2.");
        }
    }

    this->cam.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    this->cam.set(cv::CAP_PROP_FRAME_HEIGHT, 800);
    this->cam.set(cv::CAP_PROP_FPS, 10);

    // Verify what format was actually applied
    // int fourcc_val = static_cast<int>(this->cam.get(cv::CAP_PROP_FOURCC));
    // std::cout << "Actual FOURCC code: "
    //     << (char)(fourcc_val & 0xFF)
    //     << (char)((fourcc_val >> 8) & 0xFF)
    //     << (char)((fourcc_val >> 16) & 0xFF)
    //     << (char)((fourcc_val >> 24) & 0xFF) << std::endl;

    // std::cout << "Actual resolution: " << this->cam.get(cv::CAP_PROP_FRAME_WIDTH)
    //     << "x" << this->cam.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    // std::cout << "Actual FPS: " << this->cam.get(cv::CAP_PROP_FPS) << std::endl;

    // Disable autoexposure
    if (!this->cam.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25)) {
        std::cout << "AE set to 0.25 failed" << std::endl;
        if (!this->cam.set(cv::CAP_PROP_AUTO_EXPOSURE, 1)) {
            std::cout << "AE set to 1 failed" << std::endl;
            throw std::runtime_error("Could not disable camera auto-exposure.");
        }
    }

    this->cam.set(cv::CAP_PROP_EXPOSURE, 30);
    this->cam.set(cv::CAP_PROP_SATURATION, 70);
    this->cam.set(cv::CAP_PROP_GAIN, 0);
    this->cam.set(cv::CAP_PROP_AUTO_WB, 0);
    this->cam.set(cv::CAP_PROP_WB_TEMPERATURE, 5000);
    this->cam.set(cv::CAP_PROP_BRIGHTNESS, 30);

    // Give a buffer to the capture
    // this->cam.set(cv::CAP_PROP_BUFFERSIZE, 1);
}

void Camera::openCameraFeed() {
    if (!cam.isOpened())
        setupCamera();

    // create a window to display the images from the webcam
    cv::namedWindow("Video Feed of Camera " + this->cameraSavedName,
        cv::WINDOW_AUTOSIZE |
        cv::WINDOW_KEEPRATIO |
        cv::WINDOW_GUI_EXPANDED
    );

    while (true) {
        try {
            cam >> this->frame;
            // show the image on the window
            cv::imshow("Video Feed of Camera " + this->cameraSavedName, this->frame);
            // wait (10ms) for esc key to be pressed to stop
            if (cv::waitKey(10) == 27) {
                cv::destroyWindow("Video Feed of Camera " + this->cameraSavedName);
                cam.release();
                break;
            }
        }
        catch (cv::Exception ex) {
            throw std::runtime_error("Camera not available on " + this->cameraUri + " anymore.");
        }
    }
}

void Camera::takePicture() {
    if (!this->cam.isOpened())
        setupCamera();

    if (!this->cam.grab())
        throw std::runtime_error("Camera not available on " + this->cameraUri + " anymore.");

}

cv::Mat Camera::getPicture() {
    this->cam.retrieve(this->frame);
    this->cam.release(); // Needed for both cameras to work at the same time

    return this->frame;
}

void Camera::showPicture(cv::Mat image) {
    cv::namedWindow("Picture",
        cv::WINDOW_AUTOSIZE |
        cv::WINDOW_KEEPRATIO |
        cv::WINDOW_GUI_EXPANDED
    );

    while (true) {
        cv::imshow("Picture", image);
        if (cv::waitKey(10) == 27)
            break;
    }
}

std::vector<std::string> Camera::availableCameras() {
    /*
    List all cameras wired (/dev/video*) on Linux
    TODO : This will break on other systems
    */

    std::vector<std::string> medias;

#if defined(__linux__)
    for (const auto& entry : std::filesystem::directory_iterator("/dev/")) {
        std::string path = entry.path();
        if (path.find("/dev/video") != std::string::npos) {

            // More precise checks, maybe ?
            // Would be incredible to have a unique identifier for any cameras
            // Or run a Bash file that executes : udevadm info --query=all -n /dev/videoX

            medias.push_back(entry.path());
        }
    }
#elif defined(_WIN32)
    throw std::system_error()
#elif defined(__APPLE__)
    // Fuck you
    throw std::system_error()
#endif

        return medias;
}

bool Camera::save(std::string path) {
    // Writes configuration of camera
    // What the hell should be saved here ?
    // Should we also save exposure... ? or just principalPoint... ?
    return false;
}

std::ostream& operator<<(std::ostream& out, Camera const& camera) {
    return out << "Camera object :"
        << "\n  is camera open : " << camera.cam.isOpened()
        << "\n  cameraSavedName : " << camera.cameraSavedName
        << "\n  cameraUri : " << camera.cameraUri
        << "\n  focalLength : " << camera.focalLength
        << "\n  definition : (" << camera.definitionPixels.first << ";" << camera.definitionPixels.second << ") px";

}