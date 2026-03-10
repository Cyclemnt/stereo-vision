#ifndef __IMAGE___
#define __IMAGE__

#include <opencv2/opencv.hpp>

#include "provider/imageSource.hpp"
#include "files/paths.hpp"

class Image : public ImageSource
{
private:
  std::filesystem::path uri;

public:
  Image(std::filesystem::path imageUri)
  {
    uri = paths::userDevicesPath / imageUri;
  }

  /// @brief Loads the image in memory
  void takePicture() override;

  /// @brief Returns the loaded image
  /// @return Image
  const cv::Mat getPicture() const override;

  /// @brief Save the image's camera parameters/settings to a file, for later reuse
  /// @param path URI for the device config path
  /// @return was the file sucessfully written
  bool save(std::string path) const override;

  friend std::ostream &operator<<(std::ostream &, Image const &);

  // For JSON - object conversion
  friend void to_json(json &, const Image &);
  friend void from_json(const json &, Image &);
};

#endif