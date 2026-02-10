# stereo-vision

Calibration automatique

Camera peu importe

Temps réel (max caméra)

Comment vérifier la qualité des résultats
- Faire plus de calibration n'est surement pas "assez"

Faire des jeux de tests

# Timeline
Acquisition
Détection de points
Mise en correspondance
Géométrie épipolaire
Rectification
Disparité
Triangulation
Nuage de points / Mesh

## Utilisation
Image left  = stereoCam.left().grab();
Image right = stereoCam.right().grab();

std::vector<Keypoint> kp1 = detector.detect(left);
std::vector<Keypoint> kp2 = detector.detect(right);
std::vector<Descriptor> d1 = descriptor.compute(left, kp1);         ORB / SIFT
std::vector<Descriptor> d2 = descriptor.compute(right, kp2);        ORB / SIFT

Matches rawMatches = matcher.match(d1, d2);
Matches goodMatches = matchFilter.ratioTest(rawMatches);
Matches inliers = epipolarFilter.filter(kp1, kp2, goodMatches);     RANSAC

FundamentalMatrix F = GeometryEstimator::estimateFundamental(kp1, kp2, inliers);
EssentialMatrix E = GeometryEstimator::computeEssential(F, K1, K2);

RectificationMaps rectMaps = StereoRectifier::compute(K1, D1, K2, D2, R, T);
Image leftRect  = rectMaps.applyLeft(left);
Image rightRect = rectMaps.applyRight(right);

DisparityMap disparity = DisparityEstimator::compute(leftRect, rightRect);

PointCloud cloud = Triangulator::reconstruct(disparity, K1, R, T);

cloud = CloudFilter::removeOutliers(cloud);
cloud = CloudFilter::downsample(cloud);

## architecture
/vision
 ├── camera/
 │    ├── Camera
 │    ├── StereoCamera
 │    └── Calibration
 ├── features/
 │    ├── Detector
 │    ├── Descriptor
 │    └── Matcher
 ├── geometry/
 │    ├── EpipolarGeometry
 │    ├── Rectification
 │    └── Triangulation
 ├── disparity/
 │    └── DisparityEstimator
 ├── pointcloud/
 │    ├── PointCloud
 │    └── Filters
 └── pipeline/
      └── StereoReconstructionPipeline
