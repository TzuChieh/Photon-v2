#pragma once

#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/Texture/TTexture.h"
#include "Core/Texture/TSampler.h"
#include "Common/assertion.h"

#include <memory>
#include <utility>

namespace ph
{

class IsoTrowbridgeReitzTextured : public IsoTrowbridgeReitz
{
public:
	explicit IsoTrowbridgeReitzTextured(std::shared_ptr<TTexture<real>> alpha);

	real getAlpha(const SurfaceHit& X) const override;

private:
	std::shared_ptr<TTexture<real>> m_alpha;
};

// In-header Implementations:

inline IsoTrowbridgeReitzTextured::IsoTrowbridgeReitzTextured(std::shared_ptr<TTexture<real>> alpha) :
	m_alpha(std::move(alpha))
{}

inline real IsoTrowbridgeReitzTextured::getAlpha(const SurfaceHit& X) const
{
	PH_ASSERT(m_alpha);

	return TSampler<real>().sample(*m_alpha, X);
}

}// end namespace ph
