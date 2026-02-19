#include "files/fileManager.hpp"

#include <iostream>
#include <fstream>

Files::ERRORS Files::loadCameraFile(std::string cameraName, json* data) {
    assert(cameraName.size() != 0);

    std::filesystem::path filePath = paths::userCameraPath / cameraName.append(".json");
    // std::cout << "Opening Camera Config File at " << filePath << std::endl;

    std::ifstream json_file(filePath);
    if (!json_file) {
        return ERRORS::FILE_NOT_FOUND;
    }
    else if (json_file.peek() == std::ifstream::traits_type::eof()) {
        json_file.close();
        return ERRORS::FILE_EMPTY;
    }
    else {
        // files should always be non-empty before calling json::parse
        *data = json::parse(json_file);
        json_file.close();
    }
    return ERRORS::OK;
}

Files::ERRORS Files::saveCameraFile(std::string cameraName, json* data) {
    assert(cameraName.size() != 0);

    std::filesystem::create_directories(paths::userCameraPath); // Directory SHOULD exist before opening an ofstream
    std::filesystem::path filePath = paths::userCameraPath / cameraName.append(".json");

    std::ofstream json_output(filePath);
    if (!json_output.is_open()) {
        // std::cout << "Unable to open json file " << filePath << " folder" << std::endl;
        return ERRORS::UNABLE_TO_OPEN;
    }
    else {
        // std::cout << "Printing json file to " << filePath << std::endl;
        // Formatted JSON, with 4 spaces indentation
        json_output << (*data).dump(4) << std::endl;
    }
    return ERRORS::OK;

}

std::string_view Files::to_string(ERRORS err) noexcept
{
    switch (err)
    {
    case ERRORS::OK:              return "OK";
    case ERRORS::FILE_NOT_FOUND:  return "file not found";
    case ERRORS::FILE_EMPTY:      return "file is empty";
    case ERRORS::UNABLE_TO_OPEN:  return "couldn't open the file";
    case ERRORS::NO_PERMISSION:   return "file access is limited";
    default:                      return "unknown error";
    }
}