#include "Core/Receiver/EnvironmentCamera.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/Geometry/TSphere.h"
#include "Math/Random.h"
#include "Core/Quantity/Time.h"
#include "Core/Ray.h"

namespace ph
{

Spectrum EnvironmentCamera::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);

	const auto rasterRes = getRasterResolution();
	PH_ASSERT_GT(rasterRes.x, 0);
	PH_ASSERT_GT(rasterRes.y, 0);

	// Warp raster xy to surface of a unit sphere
	const auto phi    = math::constant::two_pi<float64> * rasterCoord.x / static_cast<float64>(rasterRes.x);
	const auto theta  = math::constant::pi<float64> * (1.0 - rasterCoord.y / static_cast<float64>(rasterRes.y));
	const auto outDir = math::TSphere<float64>::makeUnit().phiThetaToSurface(phi, theta);

	out_ray->setDirection(math::Vector3R(outDir).negate());
	out_ray->setOrigin(getPosition());
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	// HACK
	Time time;
	time.relativeT = math::Random::genUniformReal_i0_e1();
	out_ray->setTime(time);

	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
		"origin    = " + out_ray->getOrigin().toString() + "\n"
		"direction = " + out_ray->getDirection().toString() + "\n");

	return Spectrum(1);
}

void EnvironmentCamera::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

// command interface

EnvironmentCamera::EnvironmentCamera(const InputPacket& packet) :
	Receiver(packet)
{}

SdlTypeInfo EnvironmentCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "environment");
}

void EnvironmentCamera::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<EnvironmentCamera>(packet);
		}));
}

}// end namespace ph
