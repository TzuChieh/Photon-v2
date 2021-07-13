#include "Actor/Observer/SingleLensObserver.h"
#include "Core/Receiver/PinholeCamera.h"
#include "Core/Receiver/ThinLensCamera.h"

namespace ph
{

std::unique_ptr<Receiver> SingleLensObserver::genReceiver(const CoreCookingContext& ctx)
{
	if(m_lensRadiusMM == 0)
	{

	}
}

math::TDecomposedTransform<float64> SingleLensObserver::makeRasterToObserver() const
{
	const math::Vector2D rasterResolution(getResolution());
	const float64 sensorHeight = m_sensorWidth / getAspectRatio();

	PH_ASSERT_GT(m_sensorWidth, 0);
	PH_ASSERT_GT(sensorHeight, 0);

	m_rasterToReceiverDecomposed = math::TDecomposedTransform<float64>();
	m_rasterToReceiverDecomposed.scale(
		-m_sensorWidth / rasterResolution.x,
		-sensorHeight / rasterResolution.y,
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
