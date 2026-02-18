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

void Camera::run(std::atomic<bool>& shouldRun, std::mutex& mtx, std::chrono::time_point < std::chrono::steady_clock> startTime) {
    delete this->cameraAvailable; // Gets rid of the last mutex (needed when using a camera alone)
    this->cameraAvailable = &mtx;
    std::chrono::time_point nextShutter = startTime; // Would be a parameter
    while (shouldRun) {
        // shouldRun is owned by StereoCamera, in the main thread
        std::this_thread::sleep_until(nextShutter); // FPS of the camera captures

        this->cameraAvailable->lock();

        this->newCapture = true;
        takePicture();
        // this->cam.retrieve(this->frame);

        this->cameraAvailable->unlock();

        nextShutter += std::chrono::milliseconds(int(1 / this->fps) * 1000);

        if ((nextShutter - std::chrono::steady_clock::now()).count() > 0) {
            // nextShutter time already passed
            std::cerr << this->cameraSavedName << " is not keeping up the max-fps pace !" << std::endl;
        }
    }
}

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
    this->frame = new cv::Mat3s(800, 1280); // Might not be Mat3s for every camera. Needed for giving the size before the first frame
    this->cam.set(cv::CAP_PROP_FPS, 10);
    this->fps = 10;

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
    this->newCapture = false;

    // Sets up the rolling-window FPS counter
    assert(this->fps != 0);
    frameTimestamps.resize(
        this->fps * this->rollingWindowSeconds + 1, // This should be an integer thanks
        std::chrono::steady_clock::now()
    );

    this->computeK();
}

void Camera::computeK() {
    /*
    K = f_x   s  c_x
          0 f_y  c_y
          0   0    1
    */
    K(0, 0) = 1078;// f_x
    K(1, 1) = 1078;// f_y
    K(0, 1) = this->skew;
    K(0, 2) = this->definitionPixels.first; // c_x
    K(1, 2) = this->definitionPixels.second; // c_y
    K(2, 2) = 1;
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
            cam >> *this->frame;
            // show the image on the window
            cv::imshow("Video Feed of Camera " + this->cameraSavedName, *this->frame);
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
    /*
    Asks the camera to take a frame
    Updates the real FPS stats
    */
    if (!this->cam.isOpened())
        setupCamera();

    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Taking picture from " << this->cameraSavedName << std::endl;
    // this->cameraAvailable->lock();
    if (!this->cam.grab()) {
        // this->cameraAvailable->unlock();
        std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Taking picture from " << this->cameraSavedName << " END" << std::endl;
        throw std::runtime_error("Camera not available on " + this->cameraUri + " anymore.");
    }
    this->newCapture = true;

    // For rolling window captureFps()
    this->frameTimestamps[this->rollingWindowIndex] = std::chrono::steady_clock::now();
    this->rollingWindowIndex = (this->rollingWindowIndex + 1) % this->frameTimestamps.size();

}

cv::Mat* Camera::getPicture() {
    // TODO : I should test what happens if getPicture is called more often than takePicture()
    if (!this->newCapture) {
        // getPicture is called more often than takePicture
        // Returning the same content early to save thread time
        std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Retrieved saved picture from " << this->cameraSavedName << std::endl;

        return this->frame;
    }

    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Retrieving picture from " << this->cameraSavedName << std::endl;
    // It looks like mutexes are not needed here, and I don't understand why.
    // std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Retreived picture from " << this->cameraSavedName << std::endl;
    this->cam.retrieve(*this->frame);
    this->newCapture = false;
    // std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] Retrieved picture from " << this->cameraSavedName << " END" << std::endl;

    return this->frame;
}

double Camera::getCaptureFps() const {
    /*
    Does a rolling window measure, based on takePicture successes
    */
    size_t nextIndex = (this->rollingWindowIndex + 1) % this->frameTimestamps.size();
    std::chrono::duration<double, std::milli> msForNFrames = this->frameTimestamps[nextIndex] - this->frameTimestamps[this->rollingWindowIndex];

    return (1 / msForNFrames.count()) / this->rollingWindowSeconds * 1000;
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

bool Camera::save(std::string path) const {
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
        << "\n  advertisedFPS : " << camera.fps
        << "\n  captureFPS : " << camera.getCaptureFps()
        << "\n  definition : (" << camera.definitionPixels.first << ";" << camera.definitionPixels.second << ") px";

}

void to_json(json& j, const Camera& cam) {
    j = json({
        "test", "fuck"
        });
}

void from_json(const json& j, Camera& cam) {
    j.at("json_entry").get_to(cam.cameraSavedName);
}