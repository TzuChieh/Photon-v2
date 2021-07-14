#include "Core/Receiver/RectangularSensorReceiver.h"

namespace ph
{

RectangularSensorReceiver::RectangularSensorReceiver(
	const math::Vector2D&             sensorSize,
	const math::Transform* const      rasterToSensor,
	const math::RigidTransform* const receiverToWorld) : 

	Receiver(receiverToWorld),

	m_sensorSize    (sensorSize),
	m_rasterToSensor(rasterToSensor)
{
	PH_ASSERT(m_rasterToSensor);
}

void RectangularSensorReceiver::updateTransforms()
{
	const math::Vector2D fResolution(getRasterResolution());
	const float64 sensorHeight = m_sensorWidth / getAspectRatio();

	PH_ASSERT_GT(m_sensorWidth, 0);
	PH_ASSERT_GT(sensorHeight, 0);

	m_rasterToReceiverDecomposed = math::TDecomposedTransform<float64>();
	m_rasterToReceiverDecomposed.scale(
		-m_sensorWidth / fResolution.x,
		-sensorHeight / fResolution.y,
		1);
	m_rasterToReceiverDecomposed.translate(
		m_sensorWidth / 2,
		sensorHeight / 2,
		m_sensorOffset);

	m_rasterToReceiver = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_rasterToReceiverDecomposed));
	m_receiverToWorld  = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_receiverToWorldDecomposed));
}

}// end namespace ph
