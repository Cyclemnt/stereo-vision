#ifndef __PATHS__
#define __PATHS__

#include <string>
#include <filesystem>

namespace paths
{
    // Uses CMake defines variables
    //! Should create the directories at start
    inline const std::filesystem::path userConfigPath = std::filesystem::path(PATH_CONFIG_FILES); // Where the configurations are stored
    inline const std::filesystem::path userDevicesPath = userConfigPath / "devices";              // Where the cameras and image's camera settings will be saved/read

    inline const std::filesystem::path userDataPath = std::filesystem::path(PATH_DATA_FILES); // Where the results can be output
    inline const std::filesystem::path userPointCloudPath = userDataPath / "point_clouds";    // Where the point clouds will be exported
}

#endif