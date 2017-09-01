#include "Core/SurfaceBehavior/BSDF/IdealReflector.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/Intersection.h"

#include <iostream>
#include <cmath>

namespace ph
{

IdealReflector::IdealReflector() :
	BSDF(),
	m_fresnel(std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r))
{

}

IdealReflector::~IdealReflector() = default;

void IdealReflector::evaluate(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf,
	ESurfacePhenomenon* const out_type) const
{
	out_bsdf->set(0.0_r);
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void IdealReflector::genSample(
	const Intersection& X, const Vector3R& V,
	Vector3R* const out_L,
	SpectralStrength* const out_pdfAppliedBsdf,
	ESurfacePhenomenon* const out_type) const
{
	const Vector3R& N = X.getHitSmoothNormal();
	*out_L = V.mul(-1.0_r).reflect(N);

	const real NoL = N.dot(*out_L);
	m_fresnel->calcReflectance(NoL, out_pdfAppliedBsdf);
	out_pdfAppliedBsdf->mulLocal(1.0_r / std::abs(NoL));

	*out_type = ESurfacePhenomenon::REFLECTION;
}

void IdealReflector::calcSampleDirPdfW(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	const ESurfacePhenomenon& type,
	real* const out_pdfW) const
{
	*out_pdfW = 0.0_r;
}

}// end namespace ph