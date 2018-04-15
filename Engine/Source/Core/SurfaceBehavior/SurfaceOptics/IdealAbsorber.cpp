#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{}

IdealAbsorber::~IdealAbsorber() = default;

void IdealAbsorber::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const   out_bsdf,
	ESurfacePhenomenon* const out_type) const
{
	PH_ASSERT(out_bsdf != nullptr && out_type != nullptr);

	out_bsdf->setValues(0.0_r);
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void IdealAbsorber::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const           out_L,
	SpectralStrength* const   out_pdfAppliedBsdf,
	ESurfacePhenomenon* const out_type) const
{
	PH_ASSERT(out_pdfAppliedBsdf != nullptr && out_type != nullptr);

	out_pdfAppliedBsdf->setValues(0.0_r);
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void IdealAbsorber::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	const ESurfacePhenomenon& type,
	real* const out_pdfW) const
{
	PH_ASSERT(out_pdfW != nullptr);

	*out_pdfW = 0.0_r;
}

}// end namespace ph