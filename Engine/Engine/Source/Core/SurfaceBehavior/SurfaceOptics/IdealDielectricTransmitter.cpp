#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectricTransmitter.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

IdealDielectricTransmitter::IdealDielectricTransmitter(std::shared_ptr<DielectricFresnel> fresnel) :
	IdealDielectricTransmitter(
		std::move(fresnel), 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)))
{}

IdealDielectricTransmitter::IdealDielectricTransmitter(
	std::shared_ptr<DielectricFresnel>        fresnel,
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale) :

	SurfaceOptics(),

	m_fresnel          (std::move(fresnel)),
	m_transmissionScale(std::move(transmissionScale))
{
	PH_ASSERT(m_fresnel);
	PH_ASSERT(m_transmissionScale);

	m_phenomena.set(ESurfacePhenomenon::DeltaTransmission);
}

ESurfacePhenomenon IdealDielectricTransmitter::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DeltaTransmission;
}

void IdealDielectricTransmitter::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void IdealDielectricTransmitter::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();

	const auto optRefractDir = m_fresnel->calcRefractDir(in.getV(), N);
	if(!optRefractDir)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R L = *optRefractDir;
	const real cosI = N.dot(L);

	math::Spectrum F = m_fresnel->calcTransmittance(cosI);

	real etaI = m_fresnel->getIorOuter();
	real etaT = m_fresnel->getIorInner();
	if(N.dot(L) < 0.0_r)
	{
		std::swap(etaI, etaT);
	}

	const real relativeIor = etaT / etaI;
	out.setRelativeIor(relativeIor);

	// Account for non-symmetric scattering due to solid angle compression/expansion
	if(ctx.transport == lta::ETransport::Radiance)
	{
		F.mulLocal(relativeIor * relativeIor);
	}

	// A scale factor for artistic control
	const math::Spectrum transmissionScale =
		TSampler<math::Spectrum>().sample(*m_transmissionScale, in.getX());
	F *= transmissionScale;

	out.setPdfAppliedBsdfCos(F, std::abs(cosI));
	out.setL(L);
}

void IdealDielectricTransmitter::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});
}

}// end namespace ph
