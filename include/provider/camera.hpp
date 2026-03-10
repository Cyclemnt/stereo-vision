#ifndef __CAMERA__
#define __CAMERA__

#include <tuple>
#include <string>
#include <limits>

// For multithreading and stereo synchronization
#include <thread>
#include <atomic>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "provider/imageSource.hpp"

using K_datatype = double; // Need to be centralized, as the type is needed for JSON files (serialization)

class Camera : public ImageSource
{
private:
    // Multi-threading protections, needed once used by StereoCamera
    std::mutex *cameraAvailable; // Can't retrieve and grab at the same time (check grab when retrieve as grab protection comes from main thread, follow the flow)
    mutable bool newCapture;

    // OpenCV camera device & saved frame
    mutable cv::VideoCapture cam;
    std::string cameraUri = "/dev/video1";

    // FPS statistics
    int rollingWindowSeconds = 1;  // seconds of the rooling window (the greater this value, the smoother the counter)
    size_t rollingWindowIndex = 0; // Where next timestamp will be placed
    // frameTimestamps could be a std::array, but it's size depends on the camera fps, unknown at compile-time
    std::vector<std::chrono::steady_clock::time_point> frameTimestamps{10}; // Used for rolling-window captureFps(), default to a size of 10

    // void computeK();

public:
    // Use this when using a new camera
    Camera(std::string cameraUri, std::pair<int, int> definition, double focalLength, std::string cameraName = "Unnamed Camera")
    {
        this->cameraUri = cameraUri;
        this->definitionPixels = definition;
        this->focalLength = focalLength;
        this->deviceSavedName = cameraName;
        this->cameraAvailable = new std::mutex();
        this->cameraAvailable->unlock();
        setupCamera();
    };

    Camera(std::string cameraName)
    {
        assert(cameraName.size() != 0);
        this->deviceSavedName = cameraName;
        this->cameraAvailable = new std::mutex();
        this->cameraAvailable->unlock();

        // data = loadFile(ENUM::savedCamera, this->cameraSavedName);
        // data.focalLength
        //     data.definitionPixels_x
        //     data.definitionPixels_y
    };

    Camera() {};

    ~Camera() = default;

    /// @brief Capture infinite loop, and stores the frames in this->frame
    /// @param shouldRun Reference to a bool, to stop the infinite loop
    /// @param mtx Mutex to conserve data integrity
    /// @param startTime Time when the first capture will happen, used to syncronize with the other camera (if any)
    void run(std::atomic<bool> &shouldRun, std::mutex &mtx, std::chrono::time_point<std::chrono::steady_clock> startTime);

    /// @brief Sets all values and camera parameters to the camera device
    void setupCamera();

    void setName(std::string newName)
    {
        assert(deviceSavedName.size() != 0);
        this->deviceSavedName = newName;
    }

    void setUri(std::string newUri)
    {
        this->cameraUri = newUri;
    }

    /// @brief Getter
    /// @return Camera theoretical fps
    double getCameraFps() const { return this->fps; };
    /// @brief Compute the actual camera fps with a rolling-window approach
    /// @return Camera actual fps
    double getCaptureFps() const;

    /// @brief Opens a window to see what the camera sees. Usefull for tweaking settings and to check which camera is which
    void openCameraFeed();

    /// @brief Orders the camera device to take a picture
    void takePicture() override; // OpenCV grab

    /// @brief Save the picture taken by the camera device to memory
    /// @return Camera device image
    const cv::Mat getPicture() const override; // OpenCV retrieve

    /// @brief Save the camera parameters/settings to a file, for later reuse
    /// @param path URI for the camera path
    /// @return was the file sucessfully written
    bool save(std::string path) const override;

    /// @brief Gives you all possible uris for cameras accessible from the computer
    /// @return Vector of URIs (type /dev/video1, /dev/video2, ...)
    static std::vector<std::string> availableCameras();

    /// @brief Little usefull macro to open an image in another window
    /// @param img OpenCV image
    static void showPicture(cv::Mat img);

    friend std::ostream &operator<<(std::ostream &, Camera const &);

    // For JSON - object conversion
    friend void to_json(json &, const Camera &);
    friend void from_json(const json &, Camera &);
};

#endif