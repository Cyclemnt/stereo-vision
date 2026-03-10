#include "features/matcher.hpp"
#include <numeric>

StereoMatcher::StereoMatcher(float yTol, float ratio)
    : yThreshold(yTol), ratioTest(ratio) {}

std::vector<cv::DMatch> StereoMatcher::match(
    const std::vector<cv::KeyPoint>& kptsL, const cv::Mat& descL,
    const std::vector<cv::KeyPoint>& kptsR, const cv::Mat& descR) 
{
    // KNN matching
    cv::FlannBasedMatcher matcher;
    std::vector<std::vector<cv::DMatch>> knnMatches;
    matcher.knnMatch(descL, descR, knnMatches, 2);

    // Lowe ratio test
    const float ratio_thresh = 0.75f;
    std::vector<cv::DMatch> goodMatches;
    for (const auto& m : knnMatches) {
        if (m.size() < 2) continue;
        if (m[0].distance < ratio_thresh * m[1].distance) goodMatches.push_back(m[0]);
    }

    // If not enough matches for RANSAC, return
    if (goodMatches.size() < 8) return goodMatches;

    // Extract points
    std::vector<cv::Point2f> pts1, pts2;
    for (const auto& m : goodMatches) {
        pts1.push_back(kptsL[m.queryIdx].pt);
        pts2.push_back(kptsR[m.trainIdx].pt);
    }

    // RANSAC
    std::vector<uchar> inlierMask;
    cv::findFundamentalMat(
        pts1,
        pts2,
        cv::FM_RANSAC,
        1.0,      // reprojection threshold (pixels)
        0.99,
        inlierMask
    );

    // Keep only inliners
    std::vector<cv::DMatch> inlierMatches;
    for (size_t i = 0; i < goodMatches.size(); i++) {
        if (inlierMask[i]) inlierMatches.push_back(goodMatches[i]);
    }

    return inlierMatches;

    // const float epipolarTol = 3.0f;     // tolérance verticale
    // const float maxDisp = 128.0f;       // disparité max
    // const float minDisp = 0.0f;
    // const float ratio = 0.5f;

    // std::vector<cv::DMatch> matches;

    // // index des points droits par coordonnée Y
    // std::vector<int> orderR(kptsR.size());
    // std::iota(orderR.begin(), orderR.end(), 0);

    // std::sort(orderR.begin(), orderR.end(),
    //     [&](int a, int b){ return kptsR[a].pt.y < kptsR[b].pt.y; });

    // for (int i = 0; i < kptsL.size(); ++i) {
    //     const auto& kpL = kptsL[i];
    //     const cv::Mat dL = descL.row(i);

    //     float bestDist = FLT_MAX;
    //     float secondDist = FLT_MAX;
    //     int bestIdx = -1;

    //     for (int j : orderR) {
    //         const auto& kpR = kptsR[j];

    //         float dy = std::abs(kpL.pt.y - kpR.pt.y);
    //         if (dy > epipolarTol) {
    //             if (kpR.pt.y > kpL.pt.y + epipolarTol)
    //                 break;
    //             continue;
    //         }

    //         float disp = kpL.pt.x - kpR.pt.x;
    //         if (disp < minDisp || disp > maxDisp)
    //             continue;

    //         float dist = cv::norm(dL, descR.row(j), (descL.type() == CV_8U) ? cv::NORM_HAMMING : cv::NORM_L2);

    //         if (dist < bestDist) {
    //             secondDist = bestDist;
    //             bestDist = dist;
    //             bestIdx = j;
    //         }
    //         else if (dist < secondDist) {
    //             secondDist = dist;
    //         }
    //     }

    //     if (bestIdx >= 0 && bestDist < ratio * secondDist) {
    //         matches.emplace_back(i, bestIdx, bestDist);
    //     }
    // }

    // return matches;
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
