#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set({});
}

IdealAbsorber::~IdealAbsorber() = default;

void IdealAbsorber::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0.0_r);
}

void IdealAbsorber::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const         out_L,
	SpectralStrength* const out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	out_pdfAppliedBsdf->setValues(0.0_r);
}

void IdealAbsorber::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	real* const out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	*out_pdfW = 0.0_r;
}

}// end namespace ph