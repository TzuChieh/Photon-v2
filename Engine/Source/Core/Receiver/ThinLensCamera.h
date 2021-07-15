#pragma once

#include "Core/Receiver/RectangularSensorReceiver.h"

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
	@param receiverToWorld Transform from camera to world space.
	*/
	ThinLensCamera(
		float64                     lensRadius,
		float64                     focalDistance,
		const math::Vector2D&       sensorSize,
		const math::Transform*      rasterToSensor,
		const math::RigidTransform* cameraToWorld);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override;

private:
	float64 m_lensRadius;
	float64 m_focalDistance;

	static void genRandomSampleOnDisk(const real radius, real* const out_x, real* const out_y);
};

}// end namespace ph
