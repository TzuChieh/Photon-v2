#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"

#include <Common/assertion.h>

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

	m_phenomena.set(ESurfacePhenomenon::DeltaReflection);
}

ESurfacePhenomenon IdealReflector::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DeltaReflection;
}

void IdealReflector::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void IdealReflector::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();
	const math::Vector3R L = in.getV().mul(-1.0_r).reflect(N);
	const real NoL = N.dot(L);

	math::Spectrum F = m_fresnel->calcReflectance(NoL);

	// A scale factor for artistic control
	const math::Spectrum reflectionScale =
		TSampler<math::Spectrum>().sample(*m_reflectionScale, in.getX());
	F *= reflectionScale;

	out.setPdfAppliedBsdfCos(F, std::abs(NoL));
	out.setL(L);
}

void IdealReflector::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});
}

}// end namespace ph
