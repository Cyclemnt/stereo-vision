#include <iostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "provider/stereoSystem.hpp"
#include "features/featurePipeline.hpp"

#include "provider/calibration.hpp"
#include "geometry/rectification.hpp"
#include "features/detector.hpp"
#include "features/descriptor.hpp"
#include "features/matcher.hpp"

#include "files/fileManager.hpp"
#include <fstream> // Needed for jsonTest(). Should be removed

void availableCamTest() {
    std::vector<std::string> allCams = Camera::availableCameras();
    std::cout << "Available cameras on this computer : \n";
    for (const std::string& camPath : allCams)
        std::cout << camPath << '\n';
    std::cout << std::endl;
}

void stereoCamTest()
{
    StereoSystem<Camera> *stereo;
    {
        // Scope to limit access to leftCamera and rightCamera (they become nullptr after the std::move)
        std::unique_ptr<Camera> leftCamera = std::make_unique<Camera>("Left");
        std::unique_ptr<Camera> rightCamera = std::make_unique<Camera>("Right");

        leftCamera->setUri("/dev/video4");
        rightCamera->setUri("/dev/video2");

        leftCamera->setupCamera();
        rightCamera->setupCamera();

        stereo = new StereoSystem<Camera>(std::move(leftCamera), std::move(rightCamera));
    }
    std::cout << stereo->leftDevice() << "\n";
    std::cout << stereo->rightDevice() << std::endl;

    // stereo->openLeftCameraFeed();
    // stereo->openRightCameraFeed();
    stereo->openCameraFeeds();
    // stereo->start();

    // cv::Mat leftPic = stereo->left().getPicture();
    // std::cout << "Left pic empty : " << leftPic.empty() << std::endl;
    // cv::Mat rightPic = stereo->right().getPicture();
    // std::cout << "Right pic empty : " << rightPic.empty() << std::endl;

    // Camera::showPicture(leftPic);
    // Camera::showPicture(rightPic);

    // for (int i = 0; i < 50; i++) {
    //     std::pair<cv::Mat, cv::Mat> pair = stereo->capturePair();

    //     cv::imwrite(std::string("calibration/Left/pic") + std::to_string(
    //         std::chrono::steady_clock::now().time_since_epoch().count()
    //     ), pair.first);
    //     cv::imwrite(std::string("calibration/Right/pic") + std::to_string(
    //         std::chrono::steady_clock::now().time_since_epoch().count()
    //     ), pair.second);

    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // }

    delete stereo;

    std::cout << "deteled stereo" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void stereoImgTest()
{
    StereoSystem<Image> *stereo;
    {
        // Scope to limit access to leftCamera and rightCamera (they become nullptr after the std::move)
        std::unique_ptr<Image> leftImage = std::make_unique<Image>("l0.png");
        std::unique_ptr<Image> rightImage = std::make_unique<Image>("r0.png");

        stereo = new StereoSystem<Image>(std::move(leftImage), std::move(rightImage));
    }
    std::cout << stereo->leftDevice() << "\n";
    std::cout << stereo->rightDevice() << std::endl;

    // stereo->openLeftCameraFeed();
    // stereo->openRightCameraFeed();

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    cv::Mat leftPic = stereo->leftDevice().getPicture();
    ImageSource::showPicture(leftPic);
    // stereo->openCameraFeeds();
    // stereo->start();

    // cv::Mat leftPic = stereo->left().getPicture();
    // std::cout << "Left pic empty : " << leftPic.empty() << std::endl;
    // cv::Mat rightPic = stereo->right().getPicture();
    // std::cout << "Right pic empty : " << rightPic.empty() << std::endl;

    // Camera::showPicture(leftPic);
    // Camera::showPicture(rightPic);

    // std::this_thread::sleep_for(std::chrono::milliseconds(750));

    delete stereo;

    std::cout << "deteled stereo" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void jsonTest() {
    json data;
    Files::ERRORS status;

    // Import
    status = Files::loadCameraFile("Logitech (CO-ROB-398-CAM)", &data);
    if (status == Files::ERRORS::OK) {
        std::cout << "It worked, here is the json parsed : " << data.dump(4) << std::endl;
        Camera cam = data;
        std::cout << cam << std::endl;
    }
    else {
        std::cout << Files::to_string(status) << std::endl;
    }

    // json j;
    // j["name"] = "Lucas";
    // j["age"] = "21";
    // j["parents"] = {
    //     {"father", "Sébastien"},
    //     {"mother", "Rosine"}
    // };

    Camera left = Camera();
    left.setUri("/dev/video42");
    left.setName("Logitech (CO-ROB-398-CAM)");

    json j = left;

    std::cout << j.dump(3) << std::endl;

    // Writing json file
    status = Files::saveCameraFile("Logitech (CO-ROB-398-CAM)", &j);
    if (status == Files::ERRORS::OK) {
        std::cout << "Camera save worked !" << std::endl;
    }
    else {
        std::cout << Files::to_string(status) << std::endl;
    }
}

cv::Mat takePictureCalib(Camera *cam)
{
    cam->takePicture();
    cv::Mat picture = cam->getPicture();
    return picture;
}


cv::Mat computeSparseDisparity(const std::vector<cv::KeyPoint>& kptsL, const std::vector<cv::KeyPoint>& kptsR, const std::vector<cv::DMatch>& matches, cv::Size imgSize) {
    cv::Mat disparity = cv::Mat::zeros(imgSize, CV_32F);

    for (const auto& m : matches) {
        const auto& kpL = kptsL[m.queryIdx];
        const auto& kpR = kptsR[m.trainIdx];

        int x = (int)kpL.pt.x;
        int y = (int)kpL.pt.y;

        if (x < 0 || x >= imgSize.width || y < 0 || y >= imgSize.height) continue;

        float disp = kpL.pt.x - kpR.pt.x;
        if (disp > 0) disparity.at<float>(y, x) = disp;
    }

    return disparity;
}

void showDisparity(const cv::Mat& disparity) {
    cv::Mat dispVis;
    cv::normalize(disparity, dispVis, 0, 255, cv::NORM_MINMAX);
    dispVis.convertTo(dispVis, CV_8U);

    cv::applyColorMap(dispVis, dispVis, cv::COLORMAP_JET);

    cv::imshow("Disparity", dispVis);
    cv::waitKey(0);
}

cv::Mat computeSGBM(const cv::Mat& rectL, const cv::Mat& rectR) {
    // Paramètres de base
    int minDisparity = 0;
    int numDisparities = 128 + 32; // Doit être un multiple de 16
    int blockSize = 3;             // Taille du bloc de match

    // Création de l'objet SGBM
    int cn = rectL.channels();
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(minDisparity, numDisparities, blockSize);
    
    // Filtres post-calcul pour éviter le bruit
    // see https://docs.opencv.org/4.x/d2/d85/classcv_1_1StereoSGBM.html#ad8d3f70b8c7f0c0447014149f2bcff78a0f746667febe92e1189e924c40752660
    sgbm->setP1(8 * cn * blockSize * blockSize);
    sgbm->setP2(32 * cn * blockSize * blockSize);
    sgbm->setDisp12MaxDiff(1);           // Vérification cohérence gauche-droite (crucial pour la qualité)
    sgbm->setPreFilterCap(63);           // Coupe les valeurs extrêmes de gradient
    sgbm->setUniquenessRatio(10);        // Élimine les matches ambigus (plus c'est haut, plus c'est sévère)
    sgbm->setSpeckleWindowSize(100);     // Taille des petits "grains" de bruit à supprimer
    sgbm->setSpeckleRange(1);            // Tolérance de différence pour le débruitage
    sgbm->setMode(cv::StereoSGBM::MODE_HH);

    // Calcul de la disparité
    cv::Mat dispRaw;
    sgbm->compute(rectL, rectR, dispRaw);

    // Conversion: SGBM renvoie des valeurs signées 16 bits multipliées par 16
    cv::Mat disp32F;
    dispRaw.convertTo(disp32F, CV_32F, 1.0 / 16.0);

    return disp32F;
}

void showBeautifulMap(const cv::Mat& disp32F) {
    cv::Mat view, color;
    
    // On normalise pour que la valeur min soit à 0 et max à 255
    double min, max;
    cv::minMaxLoc(disp32F, &min, &max, nullptr, nullptr, disp32F > 0);
    disp32F.convertTo(view, CV_8U, 255.0 / (max - min), -min * 255.0 / (max - min));

    // Utilisation d'une ColorMap pour le relief
    cv::applyColorMap(view, color, cv::COLORMAP_MAGMA);
    
    // On remet en noir les zones où le calcul a échoué (valeurs <= 0)
    color.setTo(cv::Scalar(0, 0, 0), disp32F <= 0);

    cv::imshow("Disparity SGBM", color);
    cv::waitKey(0);
}

void theWholeProcess() {
    // Calibrate
    Calibrator calib("../calibration/robotnik/", {7, 5}, 30.5f);
    cv::Mat K1, D1, K2, D2, R, T;
    cv::Size imgSize;
    if (calib.extractCorners(false)) { // change to true to see each detection
        calib.compute(K1, D1, K2, D2, R, T, imgSize);
    }

    // Read
    cv::Mat imgL = cv::imread("../calibration/robotnik/pic/left.png");
    cv::Mat imgR = cv::imread("../calibration/robotnik/pic/right.png");

    // Rectify
    StereoRectifier rectifier(K1, D1, K2, D2, R, T, imgSize);
    cv::Mat rectifiedL, rectifiedR;
    rectifier.process(imgL, imgR, rectifiedL, rectifiedR);
    rectifier.vizualize(rectifiedL, rectifiedR);

    // Convert to grayscale
    cv::Mat grayL, grayR;
    cv::cvtColor(rectifiedL, grayL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rectifiedR, grayR, cv::COLOR_BGR2GRAY);

    // Boost corner detection
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8,8));
    clahe->apply(grayL, grayL);
    clahe->apply(grayR, grayR);
    // Detect keypoints 
    Detector detector;
    std::vector<cv::KeyPoint> keypoints1 = detector.detect(grayL);
    std::vector<cv::KeyPoint> keypoints2 = detector.detect(grayR);
    std::cout << keypoints1.size() << " kpts on left, " << keypoints2.size() << " kpts on right" << std::endl;
    detector.visualize(grayL, keypoints1);
    detector.visualize(grayR, keypoints2);

    // Descript keypoints
    Descriptor descriptor;
    cv::Mat descriptors1 = descriptor.compute(rectifiedL, keypoints1);
    cv::Mat descriptors2 = descriptor.compute(rectifiedR, keypoints2);

    // Match keypoints
    StereoMatcher matcher;
    std::vector<cv::DMatch> matches = matcher.match(keypoints1, descriptors1, keypoints2, descriptors2);
    std::cout << matches.size() << " matches" << std::endl;
    matcher.visualize(rectifiedL, keypoints1, rectifiedR, keypoints2, matches);

    // Compute sparse disparity map
    cv::Mat disparity = computeSparseDisparity(keypoints1, keypoints2, matches, imgSize);
    showDisparity(disparity);
    
    // From OpenCV
    auto a = computeSGBM(rectifiedL, rectifiedR);
    showBeautifulMap(a);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    theWholeProcess();

    // stereoCamTest();

    // jsonTest();

    return 0;
}
