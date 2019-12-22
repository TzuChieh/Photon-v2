#pragma once

#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"

namespace ph
{

class IsoTrowbridgeReitzConstant : public IsoTrowbridgeReitz
{
public:
	explicit IsoTrowbridgeReitzConstant(real alpha);

	real getAlpha(const SurfaceHit& X) const override;

private:
	real m_alpha;
};

// In-header Implementations:

inline IsoTrowbridgeReitzConstant::IsoTrowbridgeReitzConstant(const real alpha) :
	m_alpha(alpha > 0.001_r ? alpha : 0.001_r)
{}

inline real IsoTrowbridgeReitzConstant::getAlpha(const SurfaceHit& /* X */) const
{
	return m_alpha;
}

}// end namespace ph
