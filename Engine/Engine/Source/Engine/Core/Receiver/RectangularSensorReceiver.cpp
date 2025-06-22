#include "Engine/Core/Receiver/RectangularSensorReceiver.h"

namespace ph
{

RectangularSensorReceiver::RectangularSensorReceiver(
	const math::Vector2D&       sensorSize,
	const Transform* const      rasterToSensor,
	const RigidTransform* const cameraToWorld) :

	Receiver(cameraToWorld),

	m_sensorSize    (sensorSize),
	m_rasterToSensor(rasterToSensor)
{
	PH_ASSERT(m_rasterToSensor);
}

}// end namespace ph
