#pragma once

#include "Engine/Core/Receiver/RectangularSensorReceiver.h"
#include "Engine/Core/Quantity/TimeStep.h"

namespace ph
{

class ThinLensCamera : public RectangularSensorReceiver
{
public:
	/*!
	@param lensRadius Radius of the thin lens.
	@param focalDistance The distance to the plane this camera is focusing on.
	@param sensorSize Size of the installed sensor.
	@param rasterToSensor Transform from raster to sensor position (in camera space).
	@param cameraToWorld Transform from camera to world space.
	*/
	ThinLensCamera(
		float64               lensRadius,
		float64               focalDistance,
		const math::Vector2D& sensorSize,
		const Transform*      rasterToSensor,
		const RigidTransform* cameraToWorld,
		TimeStep              timeStep);

	math::Spectrum receiveRay(
		const math::Vector2D& rasterCoord,
		SampleFlow& sampleFlow,
		Ray* out_ray) const override;

	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override;

	const RigidTransform& getCameraToWorld() const;

private:
	float64 m_lensRadius;
	float64 m_focalDistance;
};

// In-header Implementations:

inline const RigidTransform& ThinLensCamera::getCameraToWorld() const
{
	return getReceiverToWorld();
}

}// end namespace ph
