#include "Core/Receiver/RectangularSensorReceiver.h"

namespace ph
{

RectangularSensorReceiver::RectangularSensorReceiver(
	const math::Vector2D&             sensorSize,
	const math::Transform* const      rasterToSensor,
	const math::RigidTransform* const cameraToWorld) :

	Receiver(cameraToWorld),

	m_sensorSize    (sensorSize),
	m_rasterToSensor(rasterToSensor)
{
	PH_ASSERT(m_rasterToSensor);
}

}// end namespace ph
