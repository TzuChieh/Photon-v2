#pragma once

#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/Texture/TTexture.h"
#include "Core/Texture/TSampler.h"

#include <Common/assertion.h>

#include <memory>

namespace ph
{

class IsoTrowbridgeReitzTextured : public IsoTrowbridgeReitz
{
public:
	IsoTrowbridgeReitzTextured(
		const std::shared_ptr<TTexture<real>>& alpha,
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override;

private:
	std::shared_ptr<TTexture<real>> m_alpha;
};

// In-header Implementations:

inline IsoTrowbridgeReitzTextured::IsoTrowbridgeReitzTextured(
	const std::shared_ptr<TTexture<real>>& alpha,
	const EMaskingShadowing maskingShadowingType)

	: IsoTrowbridgeReitz(maskingShadowingType)

	, m_alpha(alpha)
{}

inline std::array<real, 2> IsoTrowbridgeReitzTextured::getAlphas(const SurfaceHit& X) const
{
	PH_ASSERT(m_alpha);

	const real alpha = TSampler<real>().sample(*m_alpha, X);
	return {alpha, alpha};
}

}// end namespace ph
