#pragma once

namespace ph
{

template<typename EstimationType>
class TIRayEstimator;

class IRayEnergyEstimator;
class PathEnergyEstimator;
class BVPTEstimator;
class BNEEPTEstimator;
class BVPTDLEstimator;

enum class EEstimatorAttribute
{
	Energy,
	ShadingNormal,
	GeometryNormal,
	SurfaceParameterization,
	PrimaryHitDepth,
	AmbientOcclusion,
};

}// end namespace ph
