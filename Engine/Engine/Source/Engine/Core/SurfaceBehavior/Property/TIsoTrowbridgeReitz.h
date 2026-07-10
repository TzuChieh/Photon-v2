#pragma once

#include "Engine/Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <array>
#include <utility>

namespace ph
{

template<typename Alpha>
class TIsoTrowbridgeReitzAlpha final
{
public:
	explicit TIsoTrowbridgeReitzAlpha(Alpha alpha)
		: m_alpha(std::move(alpha))
	{}

	real get(const SurfaceHit& X) const
	{
		const real rawAlpha = m_alpha(X);
		return rawAlpha > 0.001_r ? rawAlpha : 0.001_r;
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	Alpha m_alpha;
};

template<>
class TIsoTrowbridgeReitzAlpha<TConstantSurfaceProperty<real>> final
{
public:
	explicit TIsoTrowbridgeReitzAlpha(TConstantSurfaceProperty<real> alpha)
		: m_alpha(alpha.constant > 0.001_r ? alpha.constant : 0.001_r)
	{}

	real get(const SurfaceHit& /* X */) const
	{
		return m_alpha;
	}

private:
	real m_alpha;
};

template<typename Alpha>
class TIsoTrowbridgeReitz : public IsoTrowbridgeReitz
{
	static_assert(CSurfaceProperty<Alpha, real>,
		"`Alpha` must accept `SurfaceHit` and return a real-convertible value.");

public:
	TIsoTrowbridgeReitz(
		Alpha alpha,
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override;

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	TIsoTrowbridgeReitzAlpha<Alpha> m_alpha;
};

// In-header Implementations:

template<typename Alpha>
inline TIsoTrowbridgeReitz<Alpha>::TIsoTrowbridgeReitz(
	Alpha alpha,
	const EMaskingShadowing maskingShadowingType)

	: IsoTrowbridgeReitz(maskingShadowingType)

	, m_alpha(std::move(alpha))
{}

template<typename Alpha>
inline std::array<real, 2> TIsoTrowbridgeReitz<Alpha>::getAlphas(const SurfaceHit& X) const
{
	const real alpha = m_alpha.get(X);
	return {alpha, alpha};
}

}// end namespace ph
