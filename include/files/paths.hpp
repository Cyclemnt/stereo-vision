#ifndef __PATHS__
#define __PATHS__

#include <string>
#include <filesystem>

namespace paths {
    // Uses CMake defines variables
    inline const std::filesystem::path userConfigPath = std::filesystem::path(PATH_CONFIG_FILES); // Where the cameras will be saved/read
    inline const std::filesystem::path userCameraPath = userConfigPath / "cameras"; // Where the cameras will be saved/read
    inline const std::filesystem::path userDataPath = std::filesystem::path(PATH_DATA_FILES); // Where the results can be output
    inline const std::filesystem::path userPointCloudPath = userDataPath / "point_clouds"; // Where the results can be output
}

#endif