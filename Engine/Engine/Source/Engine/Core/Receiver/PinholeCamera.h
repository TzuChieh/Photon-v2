#pragma once

#include "Engine/Core/Receiver/RectangularSensorReceiver.h"
#include "Engine/Core/Quantity/TimeStep.h"
#include "Engine/Math/TVector3.h"

namespace ph
{

class PinholeCamera : public RectangularSensorReceiver
{
public:
	/*!
	@param sensorSize Size of the installed sensor.
	@param rasterToSensor Transform from raster to sensor position (in camera space).
	@param receiverToWorld Transform from camera to world space.
	*/
	PinholeCamera(
		const math::Vector2D& sensorSize,
		const Transform*      rasterToSensor,
		const RigidTransform* cameraToWorld,
		TimeStep              timeStep);

	math::Spectrum receiveRay(
		const math::Vector2D& rasterCoord,
		SampleFlow& sampleFlow,
		Ray* out_ray) const override;
		
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

	// TODO: need Time
	//const math::Vector3R& getPinholePos() const;
	const RigidTransform& getCameraToWorld() const;

private:
	//math::Vector3R m_pinholePos;

	math::Vector3R genReceiveRayDir(const math::Vector2D& rasterCoord) const;
};

// In-header Implementations:

//inline const math::Vector3R& PinholeCamera::getPinholePos() const
//{
//	return m_pinholePos;
//}

inline const RigidTransform& PinholeCamera::getCameraToWorld() const
{
	return getReceiverToWorld();
}

}// end namespace ph
