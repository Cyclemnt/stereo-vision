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

## Solution from Ureed Hussain

[Github Repo](https://github.com/Ureed-Hussain/Computer-Vision/blob/main/Single%20camera.ipynb)

## architecture
/
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
 │    └── StereoReconstructionPipeline
 └── visualization/
      └── idk (open3D implentation ?)

## A eclaircir

Les "descripteurs" d'OpenCV, c'est quoi, comment choir entre "context window" et 128 bits binaires et autres
Ouvrir la caméra sans compression.

Comment raisonne openCV

Est-ce que nous utilisons Open3D ?
      - Propose vizualisation de nuage de point (voulu), et plein d'autres
      - Contient un GUI (d'ImGui), donc possible d'ajouter
            - Sliders pour paramètres Caméras
            - Images pour preview caméra (identifier droite/gauche et si c'est la bonne source)
      (https://www.open3d.org/docs/latest/cpp_api/)
      (https://www.open3d.org/docs/latest/tutorial/C++/cplusplus_interface.html)
      (https://grok.com/share/bGVnYWN5_2ee48377-829d-4981-9b00-87ef2ec41e14)
Ou sinon, j'ai déjà utilisé ImGui, et une extension permet de plot en 3D. Ca fonctionnerait, mais très limité pour de la vision.

## Pour la prochaine fois

Identifier toutes les variables pour cameras

      - Les caméras Microsoft LifeCam ne donnent ni la taille physique du capteur, ni la distance focale. Comment Dieu déterminer f_x et f_y ???

Géométrie du système stéréo, si les deux caméras sont coplanaires, alors la distance focale n'intervient pas dans la distance de la Baseline (R ~ Identité).
D'après Ureed Hussain, c'est un trick car les calibrations automatiques sont assez pourries.

Faire le système de sauvegarde des paramètres caméras (faire lecture ecriture fichiers de mon côté pour maintenant)

En faiant de la programmation défensive, si on rencontre un problème (genre une matrice de projection n'a pas le bon rang, on throw une exception, on retourne un false, ou on system32.shutdown()) ?

Fuck Around avec OpenCV
Faire avec OpenCV C++, la détection de points/coins/features et leurs descripteurs
Tenter le feature matching sur les images du Khizar (kitty_images/)

## Finding the camera
With ffmpeg installed, you can try to run 
```
ffplay /dev/videoX
```
and it should show you what the camera records