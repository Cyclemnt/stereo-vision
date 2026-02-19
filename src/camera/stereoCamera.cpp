#include "camera/stereoCamera.hpp"
#include "camera/camera.hpp"

StereoCamera::StereoCamera(std::string leftCameraName, std::string rightCameraName) {
    leftCam = std::make_unique<Camera>(leftCameraName);
    rightCam = std::make_unique<Camera>(rightCameraName);

    isCapturing = false;
}

StereoCamera::~StereoCamera() {
    this->stop();
}

const Camera& StereoCamera::left() const noexcept {
    return *leftCam;
}

const Camera& StereoCamera::right() const noexcept {
    return *rightCam;
}

void StereoCamera::openLeftCameraFeed() {
    this->leftCam->openCameraFeed();
}
void StereoCamera::openRightCameraFeed() {
    this->rightCam->openCameraFeed();
}
void StereoCamera::openCameraFeeds() {
    cv::namedWindow("Video Feed of Left Camera",
        cv::WINDOW_AUTOSIZE |
        cv::WINDOW_KEEPRATIO |
        cv::WINDOW_GUI_EXPANDED
    );
    cv::namedWindow("Video Feed of Right Camera",
        cv::WINDOW_AUTOSIZE |
        cv::WINDOW_KEEPRATIO |
        cv::WINDOW_GUI_EXPANDED
    );

    this->start();

    int min_fps = std::min(
        this->leftCam->getCameraFps(),
        this->rightCam->getCameraFps()
    );
    int time = 1000 / min_fps;

    while (true) {
        try {
            std::pair<cv::Mat, cv::Mat> pair = this->capturePair();
            // show the image on the window
            cv::imshow("Video Feed of Left Camera", pair.first);
            cv::imshow("Video Feed of Right Camera", pair.second);
            std::cout << "Left FPS : " << this->leftCam->getCaptureFps() << std::endl;
            std::cout << "Right FPS : " << this->rightCam->getCaptureFps() << std::endl;
            // Wait for both cameras to take another image
            if (cv::waitKey(time) == 27) {
                cv::destroyAllWindows();
                break;
            }
        }
        catch (const cv::Exception& ex) {
            throw std::runtime_error("A camera is not available.");
        }
    }

    this->stop();
}

void StereoCamera::start() {
    // Crucial metric : both pictures should be taken at the EXACT SAME time
    this->isCapturing = true;

    // Both camera thread will start at the same syncStart time.
    // Using steady_clock so it can't be modified from outside of the program
    std::chrono::time_point syncStart = std::chrono::steady_clock::now()
        + std::chrono::milliseconds(500);

    this->leftT = std::thread(
        &Camera::run,
        leftCam.get(),
        std::ref(this->isCapturing),
        std::ref(this->leftCamMtx),
        syncStart
    );
    this->rightT = std::thread(
        &Camera::run,
        rightCam.get(),
        std::ref(this->isCapturing),
        std::ref(this->rightCamMtx),
        syncStart
    );

    return;
}

void StereoCamera::stop() {
    // Ends the two camera threads
    std::cout << "Asking cameras to release" << std::endl;
    this->isCapturing = false; // Hey slaves, please behave

    // Wait for the slaves to go back to the shed
    std::cout << "Cameras are supposed to be releasing" << std::endl;
    try {
        this->leftT.join();
        this->rightT.join();
    }
    catch (const std::runtime_error& ex) {
        std::cout << "A camera might be blocked by USB restrictions";
    }
    std::cout << "Cameras sucessfully released" << std::endl;
}

std::pair<cv::Mat, cv::Mat> StereoCamera::capturePair() {
    // Takes the last taken frame of each cameras, and stores them
    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] trying to capture from main" << std::endl;
    cv::Mat imgL = *this->leftCam->getPicture();
    cv::Mat imgR = *this->rightCam->getPicture();

    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] captured from main\n" << std::endl;
    return std::make_pair(imgL, imgR);
}