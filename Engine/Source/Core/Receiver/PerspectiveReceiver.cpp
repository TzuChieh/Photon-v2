#include "Core/Receiver/PerspectiveReceiver.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void PerspectiveReceiver::updateTransforms()
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
