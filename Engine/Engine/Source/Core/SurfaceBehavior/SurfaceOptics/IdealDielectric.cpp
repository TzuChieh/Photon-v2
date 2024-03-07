#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/TSampler.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

IdealDielectric::IdealDielectric(std::shared_ptr<DielectricFresnel> fresnel) :

	IdealDielectric(
		std::move(fresnel), 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)),
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(1)))
{}

IdealDielectric::IdealDielectric(
	std::shared_ptr<DielectricFresnel>        fresnel,
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale) :

	SurfaceOptics(),

	m_fresnel          (std::move(fresnel)),
	m_reflectionScale  (std::move(reflectionScale)),
	m_transmissionScale(std::move(transmissionScale))
{
	PH_ASSERT(m_fresnel);
	PH_ASSERT(m_reflectionScale);
	PH_ASSERT(m_transmissionScale);

	m_phenomena.set({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
	m_numElementals = 2;
}

ESurfacePhenomenon IdealDielectric::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DeltaReflection :
	                                 ESurfacePhenomenon::DeltaTransmission;
}

void IdealDielectric::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void IdealDielectric::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const bool canReflect  = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();

	math::Spectrum F;
	m_fresnel->calcReflectance(N.dot(in.getV()), &F);
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

	math::Vector3R L;
	if(sampleReflect)
	{
		// Calculate reflected L
		L = in.getV().mul(-1.0_r).reflect(N).normalizeLocal();
		if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		// A scale factor for artistic control
		const math::Spectrum reflectionScale =
			TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_reflectionScale, in.getX());
		F.mulLocal(reflectionScale);

		// Account for pick probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.getV(), N, &L))
	{
		if(!ctx.sidedness.isOppositeHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		m_fresnel->calcTransmittance(N.dot(L), &F);

		if(ctx.transport == ETransport::Radiance)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}

		// A scale factor for artistic control
		const math::Spectrum transmissionScale =
			TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_transmissionScale, in.getX());
		F.mulLocal(transmissionScale);

		// Account for pick probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
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

	out.setPdfAppliedBsdf(F / N.absDot(L));
	out.setL(L);
}

void IdealDielectric::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdfW(0);
}

}// end namespace ph
