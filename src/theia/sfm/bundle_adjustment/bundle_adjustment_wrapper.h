
#include "theia/sfm/reconstruction.h"
#include "theia/sfm/bundle_adjustment/bundle_adjustment.h"
#include "theia/sfm/bundle_adjustment/bundle_adjust_two_views.h"
#include "theia/matching/feature_correspondence.h"
#include "theia/sfm/twoview_info.h"
#include "theia/sfm/camera/camera.h"
#include "theia/sfm/bundle_adjustment/optimize_relative_position_with_known_rotation.h"

namespace theia {

using Matrix3d = Eigen::Matrix<double,3,3>;
using Matrix6d = Eigen::Matrix<double,6,6>;

std::tuple<BundleAdjustmentSummary, Reconstruction> BundleAdjustReconstructionWrapper(
    const BundleAdjustmentOptions& options);

std::tuple<BundleAdjustmentSummary, Reconstruction> BundleAdjustPartialReconstructionWrapper(
    const BundleAdjustmentOptions& options,
    const std::unordered_set<ViewId>& views_to_optimize,
    const std::unordered_set<TrackId>& tracks_to_optimize);


std::tuple<BundleAdjustmentSummary, Reconstruction> BundleAdjustViewWrapper(Reconstruction& reconstruction, const BundleAdjustmentOptions& options,
                                         const ViewId view_id);


std::tuple<BundleAdjustmentSummary, Reconstruction> BundleAdjustTrackWrapper(
    Reconstruction& reconstruction, 
    const BundleAdjustmentOptions& options,
    const TrackId track_id);

std::tuple<BundleAdjustmentSummary, Camera, Camera, std::vector<Eigen::Vector4d>> BundleAdjustTwoViewsWrapper(
    const TwoViewBundleAdjustmentOptions& options,
    const std::vector<FeatureCorrespondence>& correspondences);

std::tuple<BundleAdjustmentSummary, TwoViewInfo> BundleAdjustTwoViewsAngularWrapper(
    const BundleAdjustmentOptions& options,
    const std::vector<FeatureCorrespondence>& correspondences);

std::tuple<bool, Eigen::Vector3d> OptimizeRelativePositionWithKnownRotationWrapper(
    const std::vector<FeatureCorrespondence>& correspondences,
    const Eigen::Vector3d& rotation1,
    const Eigen::Vector3d& rotation2);

// with covariance information
std::tuple<BundleAdjustmentSummary, Reconstruction, Matrix6d, double> BundleAdjustViewWithCovWrapper(
    Reconstruction& reconstruction, const BundleAdjustmentOptions& options, const ViewId view_id);

std::tuple<BundleAdjustmentSummary, Reconstruction, Matrix3d, double> BundleAdjustTrackWithCovWrapper(
    Reconstruction& reconstruction, const BundleAdjustmentOptions& options, const TrackId track_id);

}  // namespace theia
