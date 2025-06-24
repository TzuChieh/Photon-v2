#include "Engine/Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Math/math.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

ThinDielectricFilm::ThinDielectricFilm(
	std::shared_ptr<DielectricFresnel> fresnel,
	std::vector<math::SampledSpectrum> reflectanceTable,
	std::vector<math::SampledSpectrum> transmittanceTable) :

	SurfaceOptics(),

	m_fresnel           (std::move(fresnel)),
	m_reflectanceTable  (std::move(reflectanceTable)),
	m_transmittanceTable(std::move(transmittanceTable))
{
	PH_ASSERT(m_fresnel);
	PH_ASSERT_EQ(m_reflectanceTable.size(), 91);
	PH_ASSERT_EQ(m_transmittanceTable.size(), 91);

	m_phenomena.set({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
	m_numElementals = 2;
}

ESurfacePhenomenon ThinDielectricFilm::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DeltaReflection : 
	                                 ESurfacePhenomenon::DeltaTransmission;
}

void ThinDielectricFilm::calcBsdfCore(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setContributability(false);
}

void ThinDielectricFilm::genBsdfSampleCore(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const bool canReflect  = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setContributability(false);
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();

	math::Spectrum F = m_fresnel->calcReflectance(N.dot(in.getV()));
	const real reflectProb = F.avg();

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// We cannot sample both paths, choose one randomly
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
			out.setContributability(false);
			return;
		}

		scale = m_reflectanceTable[index];

		// Account for probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			scale.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit)
	{
		// TIR is already handled by the path probability, this only checks for bad configurations
		const auto optRefractDir = m_fresnel->calcRefractDir(in.getV(), N);
		if(!optRefractDir || !ctx.sidedness.isOppositeHemisphere(in.getX(), in.getV(), *optRefractDir))
		{
			out.setContributability(false);
			return;
		}

		L = *optRefractDir;
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

		/*const real relativeIor = etaT / etaI;
		out.setRelativeIor(relativeIor);*/

		// Account for probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			scale.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setContributability(false);
		return;
	}

	math::Spectrum pdfAppliedBsdfCos;
	pdfAppliedBsdfCos.setSpectral(scale.getColorValues(), math::EColorUsage::Raw);

	out.setPdfAppliedBsdfCos(pdfAppliedBsdfCos, N.absDot(L));
	out.setL(L);
}

void ThinDielectricFilm::calcBsdfPdfCore(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});
}

}// end namespace ph
