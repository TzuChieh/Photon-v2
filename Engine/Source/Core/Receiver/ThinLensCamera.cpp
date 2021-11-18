#include "Core/Receiver/ThinLensCamera.h"
#include "Core/Ray.h"
#include "Math/Transform/Transform.h"
#include "Math/Transform/RigidTransform.h"
#include "Math/Random.h"
#include "Common/assertion.h"
#include "Math/Geometry/TDisk.h"

#include <iostream>

namespace ph
{

ThinLensCamera::ThinLensCamera(
	const float64                     lensRadius,
	const float64                     focalDistance,
	const math::Vector2D&             sensorSize,
	const math::Transform*  const     rasterToSensor,
	const math::RigidTransform* const cameraToWorld) : 

	RectangularSensorReceiver(
		sensorSize, 
		rasterToSensor, 
		cameraToWorld),

	m_lensRadius   (lensRadius),
	m_focalDistance(focalDistance)
{}

math::Spectrum ThinLensCamera::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);

	// The following calculations are in camera space

	math::Vector3R sensorPos;
	getRasterToSensor().transformP(
		math::Vector3R(math::Vector3D(rasterCoord.x(), rasterCoord.y(), 0)),
		&sensorPos);

	// Subtracting lens' center position is omitted since it is at (0, 0, 0)
	const auto lensCenterToSensorDir = math::Vector3D(sensorPos.normalize());

	PH_ASSERT_GT(lensCenterToSensorDir.z(), 0);
	const auto focalPlaneDist = m_focalDistance / lensCenterToSensorDir.z();
	const auto focalPlanePos  = lensCenterToSensorDir.mul(-focalPlaneDist);

	math::TDisk<float64> lensDisk(m_lensRadius);
	const auto lensPos2D = lensDisk.sampleToSurface2D({
		math::Random::genUniformReal_i0_e1(),
		math::Random::genUniformReal_i0_e1()});
	const auto lensPos = math::Vector3D(lensPos2D.x(), lensPos2D.y(), 0);

	// Now we transform camera space data to world space

	// XXX: numerical error can be large when focalPlanePosMM is far away
	math::Vector3R worldSensedRayDir;
	getCameraToWorld().transformV(math::Vector3R(lensPos - focalPlanePos), &worldSensedRayDir);
	worldSensedRayDir.normalizeLocal();

	math::Vector3R worldLensPos;
	getCameraToWorld().transformP(math::Vector3R(lensPos), &worldLensPos);

	PH_ASSERT(out_ray);
	out_ray->setDirection(worldSensedRayDir);
	out_ray->setOrigin(worldLensPos);
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	return math::Spectrum(1);
}

void ThinLensCamera::evalEmittedImportanceAndPdfW(
	const math::Vector3R& targetPos,
	math::Vector2R* const out_filmCoord,
	math::Vector3R* const out_importance,
	real* out_filmArea,
	real* const out_pdfW) const
{
	std::cerr << "ThinLensCamera::evalEmittedImportanceAndPdfW() not implemented" << std::endl;
}

}// end namespace ph
