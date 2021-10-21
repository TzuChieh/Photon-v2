#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"

namespace ph
{

IdealTransmitter::IdealTransmitter(const std::shared_ptr<DielectricFresnel>& fresnel) :
	
	IdealTransmitter(
		fresnel, 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)))
{}

IdealTransmitter::IdealTransmitter(
	const std::shared_ptr<DielectricFresnel>&        fresnel,
	const std::shared_ptr<TTexture<math::Spectrum>>& transmissionScale) :

	SurfaceOptics(),

	m_fresnel(fresnel),
	m_transmissionScale(transmissionScale)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(transmissionScale);

	m_phenomena.set({ESurfacePhenomenon::DELTA_TRANSMISSION});
}

ESurfacePhenomenon IdealTransmitter::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DELTA_TRANSMISSION;
}

void IdealTransmitter::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.bsdf.setColorValues(0);
}

void IdealTransmitter::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.X.getShadingNormal();
	math::Vector3R& L = out.L;
	if(!m_fresnel->calcRefractDir(in.V, N, &L))
	{
		out.setMeasurability(false);
		return;
	}

	real cosI = N.dot(L);
	math::Spectrum F;
	m_fresnel->calcTransmittance(cosI, &F);

	real transportFactor = 1.0_r;
	if(ctx.transport == ETransport::RADIANCE)
	{
		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(cosI < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		transportFactor = (etaT * etaT) / (etaI * etaI);
	}
	
	out.pdfAppliedBsdf = F.mul(transportFactor / std::abs(cosI));

	// A scale factor for artistic control
	const math::Spectrum transmissionScale =
		TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_transmissionScale, in.X);
	out.pdfAppliedBsdf.mulLocal(transmissionScale);

	out.setMeasurability(true);
}

void IdealTransmitter::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph
