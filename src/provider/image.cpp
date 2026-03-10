#include "provider/image.hpp"

void Image::takePicture()
{
    frame = cv::imread(uri);
}

const cv::Mat Image::getPicture() const
{
    if (frame.size() == cv::Size(1, 1))
    {
        // Image not loaded
        throw std::runtime_error("No data is loaded, call Image::takePicture() first.");
    }
    return frame;
}

bool Image::save(std::string path) const
{
    // Should save at paths::userDevicesPath
    // Writes configuration of camera
    // What the hell should be saved here ?
    // Should we also save exposure... ? or just principalPoint... ?
    return false;
}

std::ostream &operator<<(std::ostream &out, Image const &image)
{
    return out << "Image object :"
               << "\n  deviceSavedName : " << image.deviceSavedName
               << "\n  image Uri : " << image.uri
               << "\n  calibration matrix K : " << image.K
               << "\n  focalLength : " << image.focalLength
               << "\n  definition : (" << image.definitionPixels.first << ";" << image.definitionPixels.second << ") px";
}

void to_json(json &j, const Image &cam)
{
    // Eigen matrices not being automatically cast for JSON, we implement our own
    K_datatype K_vals[9];
    // values in Eigen::Matrix<...>::data are stored column after column
    for (int i = 0; i < 3 * 3; i++)
        K_vals[i] = cam.K.data()[i];
    // So cam.K is : K_vals[0] K_vals[3] K_vals[6]
    //               K_vals[1] K_vals[4] K_vals[7]
    //               K_vals[2] K_vals[5] K_vals[8]

    j = json({
        {"Image URI", cam.uri},
        {"focalLength", cam.focalLength},
        {"skew", cam.skew},
        {"principalPoint", cam.principalPoint},
        {"fps", cam.fps},
        {"definition", cam.definitionPixels},
        {"calibration matrix", K_vals},
    });
}

void from_json(const json &j, Image &cam)
{
    j.at("Image URI").get_to(cam.uri);
    // ! Check if cam.uri points to an existing file
    j.at("focalLength").get_to(cam.focalLength);
    j.at("skew").get_to(cam.skew);
    j.at("principalPoint").get_to(cam.principalPoint);
    j.at("fps").get_to(cam.fps);
    j.at("definition").get_to(cam.definitionPixels);

    // Eigen matrices not being automatically cast for JSON, we implement our own
    K_datatype K_vals[9];
    j.at("calibration matrix").get_to(K_vals);

    for (int i = 0; i < 9; i++)
    {
        std::cout << "from_json reading of K : " << K_vals[i] << std::endl;
        cam.K(i) = K_vals[i];
    }
    // So cam.K is : K_vals[0] K_vals[3] K_vals[6]
    //               K_vals[1] K_vals[4] K_vals[7]
    //               K_vals[2] K_vals[5] K_vals[8]
}