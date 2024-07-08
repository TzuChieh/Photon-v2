#pragma once

#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"

namespace ph
{

class IsoTrowbridgeReitzConstant : public IsoTrowbridgeReitz
{
public:
	IsoTrowbridgeReitzConstant(
		real alpha,
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override;

private:
	real m_alpha;
};

// In-header Implementations:

inline IsoTrowbridgeReitzConstant::IsoTrowbridgeReitzConstant(
	const real alpha,
	const EMaskingShadowing maskingShadowingType)

	: IsoTrowbridgeReitz(maskingShadowingType)

	, m_alpha(alpha > 0.001_r ? alpha : 0.001_r)
{}

inline std::array<real, 2> IsoTrowbridgeReitzConstant::getAlphas(const SurfaceHit& /* X */) const
{
	return {m_alpha, m_alpha};
}

}// end namespace ph
