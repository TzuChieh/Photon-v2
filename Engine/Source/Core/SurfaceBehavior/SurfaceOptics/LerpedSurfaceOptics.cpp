#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Random.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"

#include "Math/Math.h"

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
}

LerpedSurfaceOptics::~LerpedSurfaceOptics() = default;

void LerpedSurfaceOptics::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf,
	ESurfacePhenomenon* const out_type) const
{
	// HACK: out_type

	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);

	SpectralStrength bsdf0, bsdf1;
	m_optics0->evalBsdf(X, L, V, &bsdf0, out_type);
	m_optics1->evalBsdf(X, L, V, &bsdf1, out_type);

	*out_bsdf = bsdf0 * ratio + bsdf1 * (SpectralStrength(1) - ratio);
}

void LerpedSurfaceOptics::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const out_L,
	SpectralStrength* const out_pdfAppliedBsdf,
	ESurfacePhenomenon* const out_type) const
{
	// HACK: out_type

	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);
	const real             prob  = pickOptics0Probability(ratio);

	const real dart = Random::genUniformReal_i0_e1();
	if(dart < prob)
	{
		SpectralStrength pdfAppliedBsdf0, bsdf1;
		m_optics0->genBsdfSample(X, V, out_L, &pdfAppliedBsdf0, out_type);
		m_optics1->evalBsdf(X, *out_L, V, &bsdf1, out_type);

		real pdfW0, pdfW1;
		m_optics0->calcBsdfSamplePdf(X, *out_L, V, *out_type, &pdfW0);
		m_optics1->calcBsdfSamplePdf(X, *out_L, V, *out_type, &pdfW1);

		const SpectralStrength bsdf = pdfAppliedBsdf0 * pdfW0 * ratio + bsdf1 * (SpectralStrength(1) - ratio);
		const real             pdfW = pdfW0 * prob + pdfW1 * (1.0_r - prob);
		*out_pdfAppliedBsdf = bsdf / pdfW;
	}
	else
	{
		SpectralStrength pdfAppliedBsdf1, bsdf0;
		m_optics1->genBsdfSample(X, V, out_L, &pdfAppliedBsdf1, out_type);
		m_optics0->evalBsdf(X, *out_L, V, &bsdf0, out_type);

		real pdfW0, pdfW1;
		m_optics0->calcBsdfSamplePdf(X, *out_L, V, *out_type, &pdfW0);
		m_optics1->calcBsdfSamplePdf(X, *out_L, V, *out_type, &pdfW1);

		const SpectralStrength bsdf = bsdf0 * ratio + pdfAppliedBsdf1 * pdfW1 * (SpectralStrength(1) - ratio);
		const real             pdfW = pdfW0 * prob + pdfW1 * (1.0_r - prob);
		*out_pdfAppliedBsdf = bsdf / pdfW;
	}
}

void LerpedSurfaceOptics::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	const ESurfacePhenomenon& type,
	real* const out_pdfW) const
{
	// HACK: type

	const SpectralStrength ratio = m_sampler.sample(*m_ratio, X);
	const real             prob  = pickOptics0Probability(ratio);

	real pdf0, pdf1;
	m_optics0->calcBsdfSamplePdf(X, L, V, type, &pdf0);
	m_optics1->calcBsdfSamplePdf(X, L, V, type, &pdf1);
	*out_pdfW = pdf0 * prob + pdf1 * (1.0_r - prob);
}

real LerpedSurfaceOptics::pickOptics0Probability(const SpectralStrength& ratio)
{
	return Math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.01_r, 1.0_r);
}

}// end namespace ph