#include "Core/Receiver/RadiantFluxPanel.h"
#include "Common/assertion.h"
#include "DataIO/SDL/InputPacket.h"
#include "DataIO/SDL/SdlLoader.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Ray.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/Random.h"

#include <limits>

namespace ph
{

Spectrum RadiantFluxPanel::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);
	PH_ASSERT_EQ(getRasterResolution().x, 1);
	PH_ASSERT_EQ(getRasterResolution().y, 1);
	PH_ASSERT(m_receiverToWorld);

	const auto halfWidth      = m_width * 0.5;
	const auto halfHeight     = m_height * 0.5;
	const auto localRectangle = math::TAABB2D<float64>({-halfWidth, -halfHeight}, {halfWidth, halfHeight});
	const auto localSurface   = localRectangle.xy01ToSurface({rasterCoord.x, rasterCoord.y});
	
	math::Vector3R surface;
	m_receiverToWorld->transformP({static_cast<real>(localSurface.y), 0, static_cast<real>(localSurface.x)}, &surface);

	const auto localHemisphere = math::THemisphere<float64>::makeUnit();

	float64 pdfW;
	const auto localDirection = localHemisphere.sampleToSurfaceCosThetaWeighted(
		{math::Random::genUniformReal_i0_e1(), math::Random::genUniformReal_i0_e1()},
		&pdfW);
	if(pdfW == 0)
	{
		return Spectrum(0);
	}

	const float64 pdfA = 1.0 / (m_width * m_height);
	if(pdfA == 0)
	{
		return Spectrum(0);
	}

	const float64 cosTheta = localDirection.y;

	math::Vector3R direction;
	m_receiverToWorld->transformV(math::Vector3R(localDirection), &direction);

	out_ray->setDirection(direction.negate());
	out_ray->setOrigin(surface);
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	// HACK
	Time time;
	time.relativeT = math::Random::genUniformReal_i0_e1();
	out_ray->setTime(time);

	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
		"origin    = " + out_ray->getOrigin().toString() + "\n"
		"direction = " + out_ray->getDirection().toString() + "\n");

	return Spectrum(static_cast<real>(cosTheta / (pdfA * pdfW)));
}

void RadiantFluxPanel::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

// Command Interface

RadiantFluxPanel::RadiantFluxPanel(const InputPacket& packet) : 

	// A radiant flux panel output single measured value, hence the 1x1 resolution
	Receiver(packet, {1, 1}),

	m_width (packet.getReal("width",  1.0_r, DataTreatment::REQUIRED())),
	m_height(packet.getReal("height", 1.0_r, DataTreatment::REQUIRED()))
{
	m_receiverToWorld = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_receiverToWorldDecomposed));
}

SdlTypeInfo RadiantFluxPanel::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "radiant-flux-panel");
}

void RadiantFluxPanel::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<RadiantFluxPanel>(packet);
		}));
}

}// end namespace ph
