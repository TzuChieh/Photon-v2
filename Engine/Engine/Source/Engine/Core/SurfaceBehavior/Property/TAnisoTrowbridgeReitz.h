#pragma once

#include "Engine/Core/SurfaceBehavior/Property/TrowbridgeReitz.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Math/constant.h"
#include "Engine/Math/math.h"

#include <Common/assertion.h>
#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <array>
#include <cmath>
#include <utility>

namespace ph
{

template<typename Alpha>
class TAnisoTrowbridgeReitzAlpha final
{
public:
	explicit TAnisoTrowbridgeReitzAlpha(Alpha alpha)
		: m_alpha(std::move(alpha))
	{}

	real get(const SurfaceHit& X) const
	{
		const real rawAlpha = m_alpha(X);
		return rawAlpha > 0.0001_r ? rawAlpha : 0.0001_r;
	}

	real getRcp2(const SurfaceHit& /* X */, const real alpha) const
	{
		return 1.0_r / (alpha * alpha);
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	Alpha m_alpha;
};

template<>
class TAnisoTrowbridgeReitzAlpha<TConstantSurfaceProperty<real>> final
{
public:
	explicit TAnisoTrowbridgeReitzAlpha(TConstantSurfaceProperty<real> alpha)
		: m_alpha    (alpha.constant > 0.0001_r ? alpha.constant : 0.0001_r)
		, m_rcpAlpha2(1.0_r / (m_alpha * m_alpha))
	{}

	real get(const SurfaceHit& /* X */) const
	{
		return m_alpha;
	}

	real getRcp2(const SurfaceHit& /* X */, real /* alpha */) const
	{
		return m_rcpAlpha2;
	}

private:
	real m_alpha;
	real m_rcpAlpha2;
};

/*!
See the original paper by Trowbridge et al. @cite Trowbridge:1975:Average. The course note
@cite Burley:2012:Physicallybased provides an in-depth description for practical applications.
*/
template<typename AlphaU, typename AlphaV>
class TAnisoTrowbridgeReitz : public TrowbridgeReitz
{
	static_assert(CSurfaceProperty<AlphaU, real>,
		"`AlphaU` must accept `SurfaceHit` and return a real-convertible value.");
	static_assert(CSurfaceProperty<AlphaV, real>,
		"`AlphaV` must accept `SurfaceHit` and return a real-convertible value.");

public:
	TAnisoTrowbridgeReitz(
		AlphaU alphaU,
		AlphaV alphaV,
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override;

	real lambda(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& unitDir,
		const std::array<real, 2>& alphas) const override;

	real distribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override;

	void sampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const override;

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	TAnisoTrowbridgeReitzAlpha<AlphaU> m_alphaU;

	[[PH_NO_UNIQUE_ADDRESS]]
	TAnisoTrowbridgeReitzAlpha<AlphaV> m_alphaV;
};

// In-header Implementations:

template<typename AlphaU, typename AlphaV>
inline TAnisoTrowbridgeReitz<AlphaU, AlphaV>::TAnisoTrowbridgeReitz(
	AlphaU                   alphaU,
	AlphaV                   alphaV,
	const EMaskingShadowing maskingShadowingType)

	: TrowbridgeReitz(maskingShadowingType)

	, m_alphaU(std::move(alphaU))
	, m_alphaV(std::move(alphaV))
{}

template<typename AlphaU, typename AlphaV>
inline std::array<real, 2> TAnisoTrowbridgeReitz<AlphaU, AlphaV>::getAlphas(
	const SurfaceHit& X) const
{
	const real alphaU = m_alphaU.get(X);
	const real alphaV = m_alphaV.get(X);
	return {alphaU, alphaV};
}

template<typename AlphaU, typename AlphaV>
inline real TAnisoTrowbridgeReitz<AlphaU, AlphaV>::lambda(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& unitDir,
	const std::array<real, 2>& alphas) const
{
	PH_ASSERT_IN_RANGE(unitDir.lengthSquared(), 0.9_r, 1.1_r);

	const real cos2Phi   = X.getDetail().getShadingBasis().cos2Phi(unitDir);
	const real sin2Phi   = 1.0_r - cos2Phi;
	const real alphaU    = alphas[0];
	const real alphaV    = alphas[1];
	const real alpha2    = cos2Phi * alphaU * alphaU + sin2Phi * alphaV * alphaV;
	const real tan2Theta = X.getDetail().getShadingBasis().tan2Theta(unitDir);
	const real sqrtTerm  = 1.0_r + alpha2 * tan2Theta;
	return 0.5_r * (-1.0_r + std::sqrt(sqrtTerm));
}

template<typename AlphaU, typename AlphaV>
inline real TAnisoTrowbridgeReitz<AlphaU, AlphaV>::distribution(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H) const
{
	// Anisotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto [alphaU, alphaV] = getAlphas(X);

	const real cosThetaH = N.dot(H);
	PH_ASSERT_GE(cosThetaH, -1e-3_r);

	const real cos2ThetaH = cosThetaH * cosThetaH;
	const real cos4ThetaH = cos2ThetaH * cos2ThetaH;
	const real sin2ThetaH = 1.0_r - cos2ThetaH;
	const real tan2ThetaH = sin2ThetaH / cos2ThetaH;
	const real cos2PhiH   = X.getDetail().getShadingBasis().cos2Phi(H);
	const real sin2PhiH   = 1.0_r - cos2PhiH;
	const real rcpAlphaU2 = m_alphaU.getRcp2(X, alphaU);
	const real rcpAlphaV2 = m_alphaV.getRcp2(X, alphaV);
	const real tanTerm    = 1.0_r + tan2ThetaH * (cos2PhiH * rcpAlphaU2 + sin2PhiH * rcpAlphaV2);
	return 1.0_r / (math::constant::pi<real> * alphaU * alphaV * cos4ThetaH * tanTerm * tanTerm);
}

template<typename AlphaU, typename AlphaV>
inline void TAnisoTrowbridgeReitz<AlphaU, AlphaV>::sampleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const std::array<real, 2>& sample,
	math::Vector3R* const out_H) const
{
	// Anisotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto [alphaU, alphaV] = getAlphas(X);

	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], 0.0_r, 1.0_r);

	const real uFactor = alphaU * std::cos(math::constant::two_pi<real> * sample[0]);
	const real vFactor = alphaV * std::sin(math::constant::two_pi<real> * sample[0]);

	const math::Vector3R zVec(X.getDetail().getShadingBasis().getZAxis().mul(uFactor));
	const math::Vector3R xVec(X.getDetail().getShadingBasis().getXAxis().mul(vFactor));

	PH_ASSERT(out_H);
	*out_H = zVec.add(xVec).mul(std::sqrt(sample[1] / (1.0_r - sample[1]))).add(N);
	*out_H = out_H->safeNormalize(N);
}

}// end namespace ph
