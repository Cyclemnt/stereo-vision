#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Charger une image
    cv::Mat image = cv::imread("test.jpg");

    if (image.empty())
    {
        std::cerr << "Erreur : impossible de charger l'image !" << std::endl;
        return 1;
    }

    // Convertir en niveaux de gris
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Afficher les images
    cv::imshow("Image originale", image);
    cv::imshow("Image en niveaux de gris", gray);

    // Attendre une touche
    cv::waitKey(0);

    return 0;
}
