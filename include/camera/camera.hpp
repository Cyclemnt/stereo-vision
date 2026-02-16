#ifndef __CAMERA__
#define __CAMERA__

#include <tuple>
#include <string>

// For multithreading and stereo synchronization
#include <thread>
#include <atomic>
#include <chrono>

#include <opencv2/opencv.hpp> 

class Camera {
private:
    std::mutex* cameraAvailable; // Can't retrieve and grab at the same time (check grab when retrieve as grab protection comes from main thread, follow the flow)
    bool newCapture;

    cv::VideoCapture cam;
    cv::Mat* frame; // I save the frame here to, later, have parallelization, so the camera will take their picture simultaneously

    std::string cameraSavedName; // Used to save camera presets, makes working with multiple cameras easier
    std::string cameraUri;
    double focalLength;
    double principalPoint;
    double fps;

    std::pair<int, int> definitionPixels;

public:
    // Use this when using a new camera 
    Camera(std::string cameraUri, std::pair<int, int> definition, double focalLength, std::string cameraName = "Unnamed Camera") {
        this->cameraUri = cameraUri;
        this->definitionPixels = definition;
        this->focalLength = focalLength;
        this->cameraSavedName = cameraName;
        this->cameraAvailable = new std::mutex();
        this->cameraAvailable->unlock();
        setupCamera();
    };

    Camera(std::string cameraName) {
        assert(cameraName.size() != 0);
        this->cameraSavedName = cameraName;
        this->cameraAvailable = new std::mutex();
        this->cameraAvailable->unlock();

        // data = loadFile(ENUM::savedCamera, this->cameraSavedName);
        // data.focalLength
        //     data.definitionPixels_x
        //     data.definitionPixels_y
    };

    ~Camera() = default;

    void run(std::atomic<bool>&, std::mutex&, std::chrono::time_point<std::chrono::steady_clock>);

    void setupCamera();

    void setName(std::string newName) {
        assert(cameraSavedName.size() != 0);
        this->cameraSavedName = newName;
    }

    void setUri(std::string newUri) {
        this->cameraUri = newUri;
    }

    int getFps() const { return this->fps; };

    void openCameraFeed();

    // Distinction between grab and retrieve : almost simultaneous pictures
    void takePicture(); // OpenCV grab
    cv::Mat* getPicture(); // OpenCV retrieve

    bool save(std::string path) const;

    static std::vector<std::string> availableCameras();
    static void showPicture(cv::Mat);

    friend std::ostream& operator<<(std::ostream&, Camera const&);
};



#endif