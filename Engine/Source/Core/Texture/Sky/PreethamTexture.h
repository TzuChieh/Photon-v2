#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/TVector3.h"

#include <memory>
#include <cmath>

namespace ph
{

class PreethamTexture : public TTexture<Spectrum>
{
public:
	PreethamTexture(
		real thetaSun,
		real turbidity);

	void sample(const SampleLocation& sampleLocation, Spectrum* out_value) const override;

private:
	real           m_thetaSun;
	math::Vector3R m_A_xyY;
	math::Vector3R m_B_xyY;
	math::Vector3R m_C_xyY;
	math::Vector3R m_D_xyY;
	math::Vector3R m_E_xyY;
	math::Vector3R m_Yabs_xyY;

	math::Vector3R F(real theta, real cosGamma) const;
};

// In-header Implementations:

inline math::Vector3R PreethamTexture::F(const real theta, const real cosGamma) const
{
	const auto rcpCosTheta = 1.0_r / std::cos(theta);

	// Gamma is the angle between direction to sun and view vector
	const auto gamma     = std::acos(cosGamma);
	const auto cos2Gamma = cosGamma * cosGamma;

	return {
		(1.0_r + m_A_xyY.x * std::exp(m_B_xyY.x * rcpCosTheta)) * (1.0_r + m_C_xyY.x * std::exp(m_D_xyY.x * gamma) + m_E_xyY.x * cos2Gamma),
		(1.0_r + m_A_xyY.y * std::exp(m_B_xyY.y * rcpCosTheta)) * (1.0_r + m_C_xyY.y * std::exp(m_D_xyY.y * gamma) + m_E_xyY.y * cos2Gamma),
		(1.0_r + m_A_xyY.z * std::exp(m_B_xyY.z * rcpCosTheta)) * (1.0_r + m_C_xyY.z * std::exp(m_D_xyY.z * gamma) + m_E_xyY.z * cos2Gamma)};
}

}// end namespace ph
