#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Random.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"

#include "Math/math.h"

namespace ph
{

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1) : 

	LerpedSurfaceOptics(
		optics0,
		optics1,
		0.5_r)
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const real ratio) : 

	LerpedSurfaceOptics(
		optics0, 
		optics1, 
		std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(ratio)))
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const std::shared_ptr<TTexture<SpectralStrength>>& ratio) : 

	m_optics0(optics0),
	m_optics1(optics1),
	m_ratio(ratio),
	m_sampler(EQuantity::ECF)
{
	PH_ASSERT(optics0 != nullptr && optics1 != nullptr && ratio != nullptr);

	m_phenomena.set({});
	m_phenomena.unionWith(m_optics0->m_phenomena);
	m_phenomena.unionWith(m_optics1->m_phenomena);
}

void LerpedSurfaceOptics::calcBsdf(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	SpectralStrength* const   out_bsdf) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);

	SpectralStrength bsdf0, bsdf1;
	m_optics0->calcBsdf(X, L, V, sidedness, &bsdf0);
	m_optics1->calcBsdf(X, L, V, sidedness, &bsdf1);

	*out_bsdf  = bsdf0 * ratio + bsdf1 * (SpectralStrength(1) - ratio);
}

void LerpedSurfaceOptics::calcBsdfSample(
	const SurfaceHit&         X,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	Vector3R* const           out_L,
	SpectralStrength* const   out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);
	const real             prob  = pickOptics0Probability(ratio);

	const real dart = Random::genUniformReal_i0_e1();
	if(dart < prob)
	{
		SpectralStrength pdfAppliedBsdf0, bsdf1;
		m_optics0->calcBsdfSample(X, V, sidedness, out_L, &pdfAppliedBsdf0);
		m_optics1->calcBsdf(X, *out_L, V, sidedness, &bsdf1);

		real pdfW0, pdfW1;
		m_optics0->calcBsdfSamplePdfW(X, *out_L, V, sidedness, &pdfW0);
		m_optics1->calcBsdfSamplePdfW(X, *out_L, V, sidedness, &pdfW1);

		const SpectralStrength bsdf = pdfAppliedBsdf0 * pdfW0 * ratio + bsdf1 * (SpectralStrength(1) - ratio);
		const real             pdfW = pdfW0 * prob + pdfW1 * (1.0_r - prob);
		*out_pdfAppliedBsdf = bsdf / pdfW;
	}
	else
	{
		SpectralStrength pdfAppliedBsdf1, bsdf0;
		m_optics1->calcBsdfSample(X, V, sidedness, out_L, &pdfAppliedBsdf1);
		m_optics0->calcBsdf(X, *out_L, V, sidedness, &bsdf0);

		real pdfW0, pdfW1;
		m_optics1->calcBsdfSamplePdfW(X, *out_L, V, sidedness, &pdfW1);
		m_optics0->calcBsdfSamplePdfW(X, *out_L, V, sidedness, &pdfW0);

		const SpectralStrength bsdf = bsdf0 * ratio + pdfAppliedBsdf1 * pdfW1 * (SpectralStrength(1) - ratio);
		const real             pdfW = pdfW0 * prob + pdfW1 * (1.0_r - prob);
		*out_pdfAppliedBsdf = bsdf / pdfW;
	}
}

void LerpedSurfaceOptics::calcBsdfSamplePdfW(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	real* const               out_pdfW) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);
	const real             prob  = pickOptics0Probability(ratio);

	real pdf0, pdf1;
	m_optics0->calcBsdfSamplePdfW(X, L, V, sidedness, &pdf0);
	m_optics1->calcBsdfSamplePdfW(X, L, V, sidedness, &pdf1);
	*out_pdfW = pdf0 * prob + pdf1 * (1.0_r - prob);
}

real LerpedSurfaceOptics::pickOptics0Probability(const SpectralStrength& ratio)
{
	return math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.01_r, 1.0_r);
}

}// end namespace ph