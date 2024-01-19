#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/Texture/TSampler.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

namespace ph
{

ThinDielectricFilm::ThinDielectricFilm(
	const std::shared_ptr<DielectricFresnel>& fresnel,
	const std::vector<math::SampledSpectrum>& reflectanceTable,
	const std::vector<math::SampledSpectrum>& transmittanceTable) :

	SurfaceOptics(),

	m_fresnel           (fresnel),
	m_reflectanceTable  (reflectanceTable),
	m_transmittanceTable(transmittanceTable)
{
	PH_ASSERT(fresnel);
	PH_ASSERT_EQ(reflectanceTable.size(), 91);
	PH_ASSERT_EQ(transmittanceTable.size(), 91);

	m_phenomena.set({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
	m_numElementals = 2;
}

ESurfacePhenomenon ThinDielectricFilm::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DeltaReflection : 
	                                 ESurfacePhenomenon::DeltaTransmission;
}

void ThinDielectricFilm::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void ThinDielectricFilm::calcBsdfSample(
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

	// We cannot sample both path, choose one randomly
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

	// Calculate reflected L
	math::Vector3R L = in.getV().mul(-1.0_r).reflect(N).normalizeLocal();

	const real degree = math::to_degrees(N.absDot(L));
	const std::size_t index = math::clamp(
		static_cast<std::size_t>(degree + 0.5_r), std::size_t(0), std::size_t(90));

	math::SampledSpectrum scale(0);
	if(sampleReflect)
	{
		if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		scale = m_reflectanceTable[index];

		// account for probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			scale.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.getV(), N, &L))
	{
		if(!ctx.sidedness.isOppositeHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		scale = m_transmittanceTable[index];

		/*if(in.transported == ETransport::RADIANCE)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(out.L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}*/

		// account for probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			scale.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setMeasurability(false);
		return;
	}

	math::Spectrum pdfAppliedBsdf;
	pdfAppliedBsdf.setSpectral((scale / N.absDot(L)).getColorValues(), math::EColorUsage::RAW);
	out.setPdfAppliedBsdf(pdfAppliedBsdf);
	out.setL(L);
	out.setMeasurability(pdfAppliedBsdf);
}

void ThinDielectricFilm::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdfW(0);
}

}// end namespace ph
