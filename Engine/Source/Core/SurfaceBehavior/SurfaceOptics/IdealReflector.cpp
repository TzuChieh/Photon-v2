#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"

#include <iostream>
#include <cmath>

namespace ph
{

IdealReflector::IdealReflector() :
	SurfaceOptics(),
	m_fresnel(std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r))
{
	m_phenomena.set({ESP::SPECULAR_REFLECTION});
}

IdealReflector::~IdealReflector() = default;

void IdealReflector::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0.0_r);
}

void IdealReflector::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const         out_L,
	SpectralStrength* const out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	const Vector3R& N = X.getShadingNormal();
	*out_L = V.mul(-1.0_r).reflect(N);

	const real NoL = N.dot(*out_L);
	m_fresnel->calcReflectance(NoL, out_pdfAppliedBsdf);
	out_pdfAppliedBsdf->mulLocal(1.0_r / std::abs(NoL));
}

void IdealReflector::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	real* const out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	*out_pdfW = 0.0_r;
}

}// end namespace ph