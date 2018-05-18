#include "Core/VolumeBehavior/ScatterFunction/HenyeyGreenstein.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

#include <cmath>

namespace ph
{
	
HenyeyGreenstein::HenyeyGreenstein(const real g) : 
	HenyeyGreenstein(std::make_shared<TConstantTexture<real>>(g))
{}

HenyeyGreenstein::HenyeyGreenstein(const std::shared_ptr<TTexture<real>>& g) :
	ScatterFunction(),
	m_g(g)
{
	PH_ASSERT(g);
}

HenyeyGreenstein::~HenyeyGreenstein() = default;

void HenyeyGreenstein::evalPhaseFunc(
	const SurfaceHit& X,
	const Vector3R&   I,
	const Vector3R&   O,
	real* const       out_pf) const
{
	PH_ASSERT(out_pf);

	const TSampler<real> sampler(EQuantity::RAW);

	const real g         = sampler.sample(*m_g, X);
	const real g2        = g * g;
	const real cosTheta  = I.dot(O);
	const real base      = 1.0_r + g2 + 2.0_r * g * cosTheta;

	*out_pf = PH_RECI_4_PI_REAL * (1.0_r - g2) / (base * std::sqrt(base));
}

}// end namespace ph