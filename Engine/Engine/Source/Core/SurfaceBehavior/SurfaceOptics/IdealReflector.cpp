#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"

#include <iostream>
#include <cmath>
#include <utility>

namespace ph
{

IdealReflector::IdealReflector(std::shared_ptr<FresnelEffect> fresnel) :

	IdealReflector(
		std::move(fresnel), 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)))
{}

IdealReflector::IdealReflector(
	std::shared_ptr<FresnelEffect>            fresnel,
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale) :

	SurfaceOptics(),

	m_fresnel        (std::move(fresnel)),
	m_reflectionScale(std::move(reflectionScale))
{
	PH_ASSERT(m_fresnel);
	PH_ASSERT(m_reflectionScale);

	m_phenomena.set({ESurfacePhenomenon::DELTA_REFLECTION});
}

ESurfacePhenomenon IdealReflector::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DELTA_REFLECTION;
}

void IdealReflector::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.bsdf.setColorValues(0);
}

void IdealReflector::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.X.getShadingNormal();
	out.L = in.V.mul(-1.0_r).reflect(N);

	const real NoL = N.dot(out.L);
	m_fresnel->calcReflectance(NoL, &(out.pdfAppliedBsdf));
	out.pdfAppliedBsdf.mulLocal(1.0_r / std::abs(NoL));

	// a scale factor for artistic control
	const math::Spectrum& reflectionScale =
		TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_reflectionScale, in.X);
	out.pdfAppliedBsdf.mulLocal(reflectionScale);

	out.setMeasurability(true);
}

void IdealReflector::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph
