
pyTheia - A Python Structure-from-Motion and Geometric Vision Library
---------------------

pyTheia is based on [TheiaSfM](http://www.theia-sfm.org).
It contains Python bindings for most of the functionalities of TheiaSfM.

**The library is still in active development and the interfaces are not yet all fixed**

With pyTheia you have access to a variety of different camera models, structure-from-motion pipelines and geometric vision algorithms.


# Differences to the original library TheiaSfM
pyTheia does not aim at being an end-to-end SfM library. For example, building robust feature detection and matching pipelines is usually application and data specific (e.g. image resolution, runtime, pose priors, invariances, ...). This includes image pre- and postprocessing. 

pyTheia is rather a "swiss knife" for quickly prototyping SfM related reconstruction applications without sacrificing perfomance.
For example SOTA feature detection & matching, place recognition algorithms are based on deep learning, and easily usable from Python. However, using these algorithms from a C++ library is not always straighforward and especially quick testing and prototyping is cumbersome.

## What was removed
Hence, we removed some libaries from the original TheiaSfM:
* SuiteSparse: Optional for ceres, however all GPL related code was removed from src/math/matrix/sparse_cholesky_llt.cc (cholmod -> Eigen::SimplicialLDLT). This will probably be slower on large problems and potentially numerically a bit more unstable.
* OpenImageIO: was used for image in and output and for recitification
* RapidJSON: Camera intrinsic in and output. Is part of cereal headers anyways
* RocksDB: Used for saving and loading extracted features efficiently

## What was added

## Examples

### Create a camera
``` Python
import pytheia as pt
prior = pt.sfm.CameraIntrinsicsPrior()
prior.focal_length.value = [focal_length]
prior.aspect_ratio.value = [aspect_ratio]
prior.principal_point.value = [cx, cy]
prior.radial_distortion.value = [k1, k2, k3, 0]
prior.tangential_distortion.value = [p1, p2]
prior.skew.value = [0]
prior.camera_intrinsics_model_type = 'PINHOLE_RADIAL_TANGENTIAL' 
#'PINHOLE', 'DOUBLE_SPHERE', 'EXTENDED_UNIFIED', 'FISHEYE', 'FOV', 'DIVISION_UNDISTORTION'
camera = pt.sfm.Camera(pt.sfm.CameraIntrinsicsModelType(1))
camera.SetFromCameraIntrinsicsPriors(prior)
```

### Solve for absolute or relative camera pose
``` Python
import pytheia as pt

# absolute pose
pose = pt.sfm.PoseFromThreePoints(pts2D, pts3D)
pose = pt.sfm.FourPointsPoseFocalLengthRadialDistortion(pts2D, pts3D)
pose = pt.sfm.FourPointPoseAndFocalLength(pts2D, pts3D)
pose = pt.sfm.DlsPnp(pts2D, pts3D)

# relative pose
pose = pt.sfm.NormalizedEightPointFundamentalMatrix(pts2D, pts2D)
pose = pt.sfm.FourPointHomography(pts2D, pts2D)
pose = pt.sfm.FivePointRelativePose(pts2D, pts2D)
pose = pt.sfm.SevenPointFundamentalMatrix(pts2D, pts2D)
```

### Create a reconstruction
Have a look at the example: sfm_pipeline.py
``` Python
import pytheia as pt
# use your favourite Feature extractor matcher 
# can also be any deep stuff
view_graph = pt.sfm.ViewGraph()
recon = pt.sfm.Reconstruction()
track_builder = pt.sfm.TrackBuilder(3, 30)

# ... match some features to find putative correspondences
success, twoview_info, inlier_indices = pt.sfm.EstimateTwoViewInfo(options, prior, prior, correspondences)
# ... get filtered feature correspondences and add them to the reconstruction
correspondences = pt.matching.FeatureCorrespondence(
            pt.sfm.Feature(point1), pt.sfm.Feature(point2))
imagepair_match = pt.matching.ImagePairMatch()
imagepair_match.image1 = img1_name
imagepair_match.image2 = img2_name
imagepair_match.twoview_info = twoview_info
imagepair_match.correspondences = correspondences
for i in range(len(verified_matches)):
  track_builder.AddFeatureCorrespondence(view_id1, correspondences[i].feature1, 
                                         view_id2, correspondences[i].feature2)

# ... Build Tracks
track_builder.BuildTracks(recon)

ptions = pt.sfm.ReconstructionEstimatorOptions()
options.num_threads = 7
options.rotation_filtering_max_difference_degrees = 10.0
options.bundle_adjustment_robust_loss_width = 3.0
options.bundle_adjustment_loss_function_type = pt.sfm.LossFunctionType(1)
options.subsample_tracks_for_bundle_adjustment = True

if reconstructiontype == 'global':
  options.filter_relative_translations_with_1dsfm = True
  reconstruction_estimator = pt.sfm.GlobalReconstructionEstimator(options)
elif reconstructiontype == 'incremental':
  reconstruction_estimator = pt.sfm.IncrementalReconstructionEstimator(options)
elif reconstructiontype == 'hybrid':
  reconstruction_estimator = pt.sfm.HybridReconstructionEstimator(options)
recon_sum = reconstruction_estimator.Estimate(view_graph, recon)

pt.io.WritePlyFile("test.ply", recon, [255,0,0],2)
pt.io.WriteReconstruction(recon, "reconstruction_file")
```


## Building
This section describes how to build on Ubuntu locally or on WSL2 both with sudo rights.
The basic dependency is:
* [http://ceres-solver.org/](ceres-solver)

Installing the ceres-solver will also install the neccessary dependencies for pyTheia:
* gflags
* glog
* Eigen

```bash
sudo apt install cmake build-essential 

# cd to your favourite library folder
mkdir LIBS
cd LIBS

# eigen
git clone https://gitlab.com/libeigen/eigen
cd eigen && git checkout 3.3.9
mkdir -p build && cd build && cmake .. && sudo make install

# libgflags libglog libatlas-base-dev
sudo apt install libgflags-dev libgoogle-glog-dev libatlas-base-dev

# ceres solver
cd LIBS
git clone https://ceres-solver.googlesource.com/ceres-solver
cd ceres-solver && git checkout 2.0.0 && mkdir build && cd build
cmake .. -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_BENCHMARKS=OFF
make -j && make install
```

## How to build Python wheels

### Local build
```bash
python setup.py bdist_wheel
```

### With docker
The docker build will actually build manylinux wheels. (Python 3.5-3.9)
```bash
docker build -t pytheia:0.1 .
docker run -it pytheia:0.1
```
Then all the wheels will be inside the container in the folder /home/wheelhouse.
Open a second terminal and run
```bash
docker ps # this will give you a list of running containers to find the correct CONTAINER_ID
docker cp CONTAINER_ID:/home/wheelhouse /path/to/result/folder/pytheia_wheels
```