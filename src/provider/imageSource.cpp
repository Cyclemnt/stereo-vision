#include "provider/imageSource.hpp"

void ImageSource::showPicture(const cv::Mat &image)
{
    cv::namedWindow("Picture",
                    cv::WINDOW_AUTOSIZE |
                        cv::WINDOW_KEEPRATIO |
                        cv::WINDOW_GUI_EXPANDED);

    cv::imshow("Picture", image);
    while (true)
    {
        if (cv::waitKey(10) == 27)
            break;
    }
}