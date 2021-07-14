#pragma once

#include "Core/Receiver/RectangularSensorReceiver.h"
#include "Math/TVector3.h"

namespace ph
{

class PinholeCamera : public RectangularSensorReceiver
{
public:
	PinholeCamera(
		const math::Vector2D&       sensorSize,
		const math::Transform*      rasterToSensor,
		const math::RigidTransform* receiverToWorld);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

	// TODO: need Time
	//const math::Vector3R& getPinholePos() const;

private:
	//math::Vector3R m_pinholePos;

	math::Vector3R genReceiveRayDir(const math::Vector2D& rasterCoord) const;
};

// In-header Implementations:

//inline const math::Vector3R& PinholeCamera::getPinholePos() const
//{
//	return m_pinholePos;
//}

}// end namespace ph
