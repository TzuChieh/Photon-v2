#include "Core/Texture/Sky/PreethamTexture.h"
#include "Math/constant.h"
#include "Math/Geometry/TSphere.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Color/color_spaces.h"
#include "Common/logging.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PreethamTexture, Texture);

PreethamTexture::PreethamTexture(
	const real phiSun,
	const real thetaSun,
	const real turbidity,
	const real energyScale) :

	TTexture(),

	m_phiSun     (phiSun),
	m_thetaSun   (thetaSun),
	m_energyScale(energyScale)
{
	const auto T = turbidity;

	m_A_xyY = {
		-0.0193_r * T - 0.2592_r,
		-0.0167_r * T - 0.2608_r,
		 0.1787_r * T - 1.4630_r};

	m_B_xyY = {
		-0.0665_r * T + 0.0008_r,
		-0.0950_r * T + 0.0092_r,
		-0.3554_r * T + 0.4275_r};

	m_C_xyY = {
		-0.0004_r * T + 0.2125_r,
		-0.0079_r * T + 0.2102_r,
		-0.0227_r * T + 5.3251_r};

	m_D_xyY = {
		-0.0641_r * T - 0.8989_r,
		-0.0441_r * T - 1.6537_r,
		 0.1206_r * T - 2.5771_r};

	m_E_xyY = {
		-0.0033_r * T + 0.0452_r,
		-0.0109_r * T + 0.0529_r,
		-0.0670_r * T + 0.3703_r};

	// Calculating absolute value of zenith luminance

	const auto chi = (4.0_r / 9.0_r - T / 120.0_r) * (math::constant::pi<real> - 2.0_r * thetaSun);
	const auto Yz  = (4.0453_r * T - 4.9710_r) * std::tan(chi) - 0.2155_r * T + 2.4192_r;

	const math::Vector3R vecT = {T * T, T, 1.0_r};

	const real thetaSun2 = thetaSun * thetaSun;
	const real thetaSun3 = thetaSun * thetaSun * thetaSun;

	const math::Vector3R vecSx = {
		 0.0017_r * thetaSun3 - 0.0037_r * thetaSun2 + 0.0021_r * thetaSun + 0.000_r,
		-0.0290_r * thetaSun3 + 0.0638_r * thetaSun2 - 0.0320_r * thetaSun + 0.0039_r,
		 0.1169_r * thetaSun3 - 0.2120_r * thetaSun2 + 0.0605_r * thetaSun + 0.2589_r};

	const math::Vector3R vecSy = {
		 0.0028_r * thetaSun3 - 0.0061_r * thetaSun2 + 0.0032_r * thetaSun + 0.000_r,
		-0.0421_r * thetaSun3 + 0.0897_r * thetaSun2 - 0.0415_r * thetaSun + 0.0052_r,
		 0.1535_r * thetaSun3 - 0.2676_r * thetaSun2 + 0.0667_r * thetaSun + 0.2669_r};

	const auto xz = vecT.dot(vecSx);
	const auto yz = vecT.dot(vecSy);

	// <Yz> is in luminance in K*cd/m^2
	m_Yabs_xyY = {xz, yz, Yz};

	if(m_Yabs_xyY.z() <= 0)
	{
		PH_LOG_WARNING(PreethamTexture,
			"turbidity = {} causes absolute zenith luminance to be negative/zero "
			"(currently = {} K*cd/m^2); consider using values in [2, 10]", 
			turbidity, m_Yabs_xyY.z());
	}
}

void PreethamTexture::sample(const SampleLocation& sampleLocation, math::Spectrum* const out_value) const
{
	PH_ASSERT(out_value);

	// Phi & theta for viewing direction
	const auto viewPhiTheta = math::TSphere<real>::makeUnit().latLong01ToPhiTheta(sampleLocation.uv());

	// Model is defined for top hemisphere only
	auto theta = viewPhiTheta.y();
	if(theta >= 0.5_r *  math::constant::pi<real> || theta < 0.0_r)
	{
		out_value->setColorValues(0);
		return;
	}

	// Avoid division by zero later
	theta = std::min(theta, 0.499_r * math::constant::pi<real>);

	const auto viewDir = math::TSphere<real>::makeUnit().phiThetaToSurface(viewPhiTheta);
	const auto sunDir  = math::TSphere<real>::makeUnit().phiThetaToSurface({m_phiSun, m_thetaSun});

	// Gamma is the angle between direction to sun and view vector
	const auto cosGamma = sunDir.dot(viewDir);

	const math::Vector3R F_view   = F(theta, cosGamma);
	const math::Vector3R F_zenith = F(0.0_r, std::cos(m_thetaSun));

	const auto Y_xyY = m_Yabs_xyY.mul(F_view.div(F_zenith));

	// Convert from K*cd/m^2 to radiance
	math::Vector3R radiance_xyY = {Y_xyY.x(), Y_xyY.y(), Y_xyY.z() * 1000.0_r * 0.0079_r};

	// Apply non-physical scale factor
	radiance_xyY.z() *= m_energyScale;

	out_value->setTransformed<math::EColorSpace::CIE_xyY>(radiance_xyY.toArray(), math::EColorUsage::EMR);

	if(!out_value->isFinite())
	{
		out_value->setColorValues(0);
	}
}

}// end namespace ph
