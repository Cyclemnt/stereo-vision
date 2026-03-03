#include "features/matcher.hpp"

StereoMatcher::StereoMatcher(float yTol, float ratio)
    : yThreshold(yTol), ratioTest(ratio) {}

// From LLM, to be revised
std::vector<cv::DMatch> StereoMatcher::match(
    const std::vector<cv::KeyPoint>& kptsL, const cv::Mat& descL,
    const std::vector<cv::KeyPoint>& kptsR, const cv::Mat& descR) 
{
    // 1. Brute Force Matcher avec Distance de Hamming
    cv::BFMatcher bf(cv::NORM_HAMMING, false); // false = pas de cross-check interne pour le faire à la main
    
    // On récupère les 2 meilleurs pour le ratio test
    std::vector<std::vector<cv::DMatch>> knnMatches;
    bf.knnMatch(descL, descR, knnMatches, 2);

    std::vector<cv::DMatch> ratioMatches;
    for (auto& m : knnMatches) {
        if (m.size() < 2) continue;
        // Ratio test de Lowe
        if (m[0].distance < m[1].distance * 0.75f) {
            // Contrainte épipolaire manuelle
            float yDiff = std::abs(kptsL[m[0].queryIdx].pt.y - kptsR[m[0].trainIdx].pt.y);
            if (yDiff < 3.0f) { // Tolérance un peu plus large vu ton RMS
                ratioMatches.push_back(m[0]);
            }
        }
    }

    // 2. Filtre RANSAC (Le "Nettoyeur")
    if (ratioMatches.size() < 8) return ratioMatches;

    std::vector<cv::Point2f> ptsL, ptsR;
    for (auto& m : ratioMatches) {
        ptsL.push_back(kptsL[m.queryIdx].pt);
        ptsR.push_back(kptsR[m.trainIdx].pt);
    }

    std::vector<uchar> inliersMask;
    // On cherche la matrice Fondamentale. En théorie elle est simple (rectifiée), 
    // mais RANSAC va trouver les points qui mentent.
    cv::findFundamentalMat(ptsL, ptsR, cv::FM_RANSAC, 1.0, 0.99, inliersMask);

    std::vector<cv::DMatch> finalMatches;
    for (size_t i = 0; i < inliersMask.size(); i++) {
        if (inliersMask[i]) {
            finalMatches.push_back(ratioMatches[i]);
        }
    }

    return finalMatches;
}

void StereoMatcher::visualize(const cv::Mat& img1, const std::vector<cv::KeyPoint>& kp1, const cv::Mat& img2, const std::vector<cv::KeyPoint>& kp2, const std::vector<cv::DMatch>& matches) const {
    cv::Mat output;

    cv::drawMatches(
        img1, kp1,
        img2, kp2,
        matches,
        output,
        cv::Scalar::all(-1),  // auto color
        cv::Scalar::all(-1),
        std::vector<char>(),
        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
    );

    cv::imshow("Matches", output);
    cv::waitKey(0);
}
