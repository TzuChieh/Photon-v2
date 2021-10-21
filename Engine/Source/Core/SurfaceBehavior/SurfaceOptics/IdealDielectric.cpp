#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"
#include "Core/SampleGenerator/SampleFlow.h"

namespace ph
{

IdealDielectric::IdealDielectric(const std::shared_ptr<DielectricFresnel>& fresnel) :

	IdealDielectric(
		fresnel, 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)),
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)))
{}

IdealDielectric::IdealDielectric(
	const std::shared_ptr<DielectricFresnel>&        fresnel,
	const std::shared_ptr<TTexture<math::Spectrum>>& reflectionScale,
	const std::shared_ptr<TTexture<math::Spectrum>>& transmissionScale) :

	SurfaceOptics(),

	m_fresnel          (fresnel),
	m_reflectionScale  (reflectionScale),
	m_transmissionScale(transmissionScale)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(reflectionScale);
	PH_ASSERT(transmissionScale);

	m_phenomena.set({ESurfacePhenomenon::DELTA_REFLECTION, ESurfacePhenomenon::DELTA_TRANSMISSION});
	m_numElementals = 2;
}

ESurfacePhenomenon IdealDielectric::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DELTA_REFLECTION : 
	                                 ESurfacePhenomenon::DELTA_TRANSMISSION;
}

void IdealDielectric::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.bsdf.setColorValues(0);
}

void IdealDielectric::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const bool canReflect  = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.X.getShadingNormal();

	math::Spectrum F;
	m_fresnel->calcReflectance(N.dot(in.V), &F);
	const real reflectProb = F.avg();

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// We cannot sample both path, choose one stochastically
	if(sampleReflect && sampleTransmit)
	{
		if(sampleFlow.unflowedPick(reflectProb))
		{
			sampleTransmit = false;
		}
		else
		{
			sampleReflect = false;
		}
	}

	PH_ASSERT(sampleReflect || sampleTransmit);

	if(sampleReflect)
	{
		// calculate reflected L
		out.L = in.V.mul(-1.0_r).reflect(N).normalizeLocal();
		if(!ctx.sidedness.isSameHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		// a scale factor for artistic control
		const math::Spectrum reflectionScale =
			TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_reflectionScale, in.X);
		F.mulLocal(reflectionScale);

		// account for probability
		if(ctx.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.V, N, &(out.L)))
	{
		if(!ctx.sidedness.isOppositeHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		// FIXME: just use 1 - F
		m_fresnel->calcTransmittance(N.dot(out.L), &F);

		if(ctx.transport == ETransport::RADIANCE)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(out.L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}

		// a scale factor for artistic control
		const math::Spectrum transmissionScale =
			TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_transmissionScale, in.X);
		F.mulLocal(transmissionScale);

		// account for probability
		if(ctx.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setMeasurability(false);
		return;
	}

	out.pdfAppliedBsdf = F / N.absDot(out.L);
	out.setMeasurability(true);
}

void IdealDielectric::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph
