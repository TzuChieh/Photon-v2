#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/TSampler.h"

#include <iostream>
#include <cmath>

namespace ph
{

IdealReflector::IdealReflector(const std::shared_ptr<FresnelEffect>& fresnel) :

	IdealReflector(
		fresnel, 
		std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(1.0_r)))
{}

IdealReflector::IdealReflector(
	const std::shared_ptr<FresnelEffect>&              fresnel,
	const std::shared_ptr<TTexture<SpectralStrength>>& reflectionScale) : 

	SurfaceOptics(),

	m_fresnel(fresnel),
	m_reflectionScale(reflectionScale)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(reflectionScale);

	m_phenomena.set({ESurfacePhenomenon::DELTA_REFLECTION});
}

ESurfacePhenomenon IdealReflector::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DELTA_REFLECTION;
}

void IdealReflector::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	out.bsdf.setValues(0.0_r);
}

void IdealReflector::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const Vector3R& N = in.X.getShadingNormal();
	out.L = in.V.mul(-1.0_r).reflect(N);

	const real NoL = N.dot(out.L);
	m_fresnel->calcReflectance(NoL, &(out.pdfAppliedBsdf));
	out.pdfAppliedBsdf.mulLocal(1.0_r / std::abs(NoL));

	// a scale factor for artistic control
	const SpectralStrength& reflectionScale = 
		TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_reflectionScale, in.X);
	out.pdfAppliedBsdf.mulLocal(reflectionScale);

	out.setValidity(true);
}

void IdealReflector::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph