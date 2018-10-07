#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set({});
}

void IdealAbsorber::calcBsdf(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	SpectralStrength* const   out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0.0_r);
}

void IdealAbsorber::calcBsdfSample(
	const SurfaceHit&         X,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	Vector3R* const           out_L,
	SpectralStrength* const   out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	out_pdfAppliedBsdf->setValues(0.0_r);
}

void IdealAbsorber::calcBsdfSamplePdfW(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	real* const               out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	*out_pdfW = 0.0_r;
}

}// end namespace ph