#include "provider/stereoSystem.hpp"

template <ImageProvider Provider>
StereoSystem<Provider>::StereoSystem(std::string leftImageName, std::string rightImageName)
{
    leftDev = std::make_unique<Provider>(leftImageName);
    rightDev = std::make_unique<Provider>(rightImageName);

    isCapturing = false;
}

template <ImageProvider Provider>
StereoSystem<Provider>::~StereoSystem()
{
    this->stop();
}

template <ImageProvider Provider>
void StereoSystem<Provider>::openLeftCameraFeed()
{
    if constexpr (std::same_as<Provider, Camera>)
    {
        (*this->leftDev).openCameraFeed();
    }
}

template <ImageProvider Provider>
void StereoSystem<Provider>::openRightCameraFeed()
{
    if constexpr (std::same_as<Provider, Camera>)
    {
        (*this->rightDev).openCameraFeed();
    }
}

template <ImageProvider Provider>
void StereoSystem<Provider>::openCameraFeeds()
{
    if constexpr (std::same_as<Provider, Camera>)
    {
        cv::namedWindow("Video Feed of Left Camera",
                        cv::WINDOW_AUTOSIZE |
                            cv::WINDOW_KEEPRATIO |
                            cv::WINDOW_GUI_EXPANDED);
        cv::namedWindow("Video Feed of Right Camera",
                        cv::WINDOW_AUTOSIZE |
                            cv::WINDOW_KEEPRATIO |
                            cv::WINDOW_GUI_EXPANDED);

        this->start();

        int min_fps = std::min(
            this->leftDev->getCameraFps(),
            this->rightDev->getCameraFps());
        int time = 1000 / min_fps;

        while (true)
        {
            try
            {
                std::pair<cv::Mat, cv::Mat> pair = this->capturePair();
                // show the image on the window
                cv::imshow("Video Feed of Left Camera", pair.first);
                cv::imshow("Video Feed of Right Camera", pair.second);
                std::cout << "Left FPS : " << this->leftDev->getCaptureFps() << std::endl;
                std::cout << "Right FPS : " << this->rightDev->getCaptureFps() << std::endl;
                // Wait for both cameras to take another image
                if (cv::waitKey(time) == 27)
                {
                    cv::destroyAllWindows();
                    break;
                }
            }
            catch (const cv::Exception &ex)
            {
                throw std::runtime_error("A camera is not available.");
            }
        }

        this->stop();
    }
}

template <ImageProvider Provider>
void StereoSystem<Provider>::start()
{
    if constexpr (std::same_as<Provider, Camera>)
    {
        // There is no reason to ever start images

        // Crucial metric : both pictures should be taken at the EXACT SAME time
        this->isCapturing = true;

        // Both camera thread will start at the same syncStart time.
        // Using steady_clock so it can't be modified from outside of the program
        std::chrono::time_point syncStart = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);

        this->leftT = std::thread(
            &Camera::run,
            leftDev.get(),
            std::ref(this->isCapturing),
            std::ref(this->leftCamMtx),
            syncStart);
        this->rightT = std::thread(
            &Camera::run,
            rightDev.get(),
            std::ref(this->isCapturing),
            std::ref(this->rightCamMtx),
            syncStart);
    }
    return;
}

template <ImageProvider Provider>
void StereoSystem<Provider>::stop()
{
    if constexpr (std::same_as<Provider, Camera>) // Wow constexpr, this seems like some fidgetty shit
    {
        // There is no reason to ever start images

        // Ends the two camera threads
        std::cout << "Asking cameras to release" << std::endl;
        this->isCapturing = false; // Hey slaves, please behave

        // Wait for the slaves to go back to the shed
        std::cout << "Cameras are supposed to be releasing" << std::endl;
        try
        {
            this->leftT.join();
            this->rightT.join();
        }
        catch (const std::runtime_error &ex)
        {
            std::cout << "A camera might be blocked by USB restrictions";
        }
        std::cout << "Cameras successfully released" << std::endl;
    }
    return;
}

template <ImageProvider Provider>
std::pair<cv::Mat, cv::Mat> StereoSystem<Provider>::capturePair()
{
    // Takes the last taken frame of each cameras, and stores them
    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] trying to capture from main" << std::endl;
    cv::Mat imgL = this->leftDev->getPicture();
    cv::Mat imgR = this->rightDev->getPicture();

    std::cout << "[" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "] captured from main\n"
              << std::endl;
    return std::make_pair(imgL, imgR);
}

template class StereoSystem<Camera>;
template class StereoSystem<Image>;
