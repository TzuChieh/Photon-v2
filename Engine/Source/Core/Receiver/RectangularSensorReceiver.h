#pragma once

#include "Core/Receiver/Receiver.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"

namespace ph::math { class math::Transform; }
namespace ph::math { class math::RigidTransform; }

namespace ph
{

class Ray;

class RectangularSensorReceiver : public Receiver
{
public:
	RectangularSensorReceiver(
		const math::Vector2D&       sensorSize, 
		const math::Transform*      rasterToSensor,
		const math::RigidTransform* receiverToWorld);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override = 0;

	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override = 0;

	const math::Vector2D& getSensorSize() const;
	const math::Transform& getRasterToSensor() const;

private:
	math::Vector2D         m_sensorSize;
	const math::Transform* m_rasterToSensor;
};

// In-header Implementations:

inline const math::Vector2D& RectangularSensorReceiver::getSensorSize() const
{
	return m_sensorSize;
}

inline const math::Transform& RectangularSensorReceiver::getRasterToSensor() const
{
	PH_ASSERT(m_rasterToSensor);

	return *m_rasterToSensor;
}

}// end namespace ph
