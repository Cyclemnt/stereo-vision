#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "files/paths.hpp"

/*
Static functions to read/write config/data files
*/

namespace Files {

    /// @brief Error codes for reading/writing files. Should be managed by each process using the Files namespace
    enum class ERRORS {
        OK = 0,
        FILE_NOT_FOUND,
        FILE_EMPTY,
        NO_PERMISSION,
        UNABLE_TO_OPEN
    };

    /// @brief Loads a camera configuration file
    /// @param cameraName The name of the saved camera (also the name of the configuration file + ".json")
    /// @param data Returned data read from the file, in JSON
    /// @return Error code, see enum class @ref Files::ERRORS
    ERRORS loadCameraFile(std::string cameraName, json* data);
    ERRORS saveCameraFile(std::string cameraName, json* data);

    std::string_view to_string(ERRORS err) noexcept;
}

#endif