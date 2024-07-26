#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricShell.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/constant_textures.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <cmath>
#include <utility>

namespace ph
{

ThinDielectricShell::ThinDielectricShell(std::shared_ptr<DielectricFresnel> fresnel)
	: ThinDielectricShell(
		std::move(fresnel),
		std::make_shared<TConstantTexture<real>>(0.0_r),
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(0)))
{}

ThinDielectricShell::ThinDielectricShell(
	std::shared_ptr<DielectricFresnel>        fresnel,
	std::shared_ptr<TTexture<real>>           thickness,
	std::shared_ptr<TTexture<math::Spectrum>> sigmaT)

	: ThinDielectricShell(
		std::move(fresnel),
		std::move(thickness),
		std::move(sigmaT),
		nullptr,
		nullptr)
{}

ThinDielectricShell::ThinDielectricShell(
	std::shared_ptr<DielectricFresnel>        fresnel,
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale)

	: ThinDielectricShell(
		std::move(fresnel),
		std::make_shared<TConstantTexture<real>>(0.0_r),
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(0)),
		std::move(reflectionScale),
		std::move(transmissionScale))
{}

ThinDielectricShell::ThinDielectricShell(
	std::shared_ptr<DielectricFresnel>        fresnel,
	std::shared_ptr<TTexture<real>>           thickness,
	std::shared_ptr<TTexture<math::Spectrum>> sigmaT,
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale)

	: SurfaceOptics()

	, m_fresnel          (std::move(fresnel))
	, m_thickness        (std::move(thickness))
	, m_sigmaT           (std::move(sigmaT))
	, m_reflectionScale  (std::move(reflectionScale))
	, m_transmissionScale(std::move(transmissionScale))
{
	PH_ASSERT(m_fresnel);
	PH_ASSERT(m_thickness);
	PH_ASSERT(m_sigmaT);

	m_phenomena.set({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
	m_numElementals = 2;
}

ESurfacePhenomenon ThinDielectricShell::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::DeltaReflection : 
	                                 ESurfacePhenomenon::DeltaTransmission;
}

void ThinDielectricShell::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void ThinDielectricShell::genBsdfSample(
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

	// Single bounce reflectance and transmittance
	math::Spectrum R = m_fresnel->calcReflectance(N.dot(in.getV()));
	math::Spectrum T = R.complement();

	// Scale factors for artistic control
	R *= TSampler<math::Spectrum>().sampleOrDefault(m_reflectionScale.get(), in.getX(), math::Spectrum(1));
	T *= TSampler<math::Spectrum>().sampleOrDefault(m_transmissionScale.get(), in.getX(), math::Spectrum(1));
	
	// Single penetration attenuation
	math::Spectrum A(1);

	// Attenuation by Beer's law
	const auto optInnerCos = m_fresnel->calcRefractCos(in.getV(), N);
	if(optInnerCos)
	{
		const real penetrationDepth = TSampler<real>().sample(*m_thickness, in.getX()) / std::abs(*optInnerCos);
		const auto sigmaT = TSampler<math::Spectrum>().sample(*m_sigmaT, in.getX());
		A *= math::Spectrum().exp(-penetrationDepth * sigmaT);
	}
	
	const math::Spectrum RA = R * A;
	const math::Spectrum RA2 = RA * RA;
	const math::Spectrum TAT = T * A * T;

	// Infinite bounce reflectance: R + TARAT + TARARARAT + ...
	math::Spectrum infR = R + (TAT * RA) / (1.0_r - RA2);

	// Any TIR would result in a single R term
	if(!infR.isFinite())
	{
		infR = R;
	}

	bool sampleReflect = canReflect;
	bool sampleTransmit = canTransmit;

	// We cannot sample both paths, choose one stochastically
	real pathProb = 1.0_r;
	if(sampleReflect && sampleTransmit)
	{
		const real reflectProb = infR.avg();
		if(sampleFlow.unflowedPick(reflectProb))
		{
			pathProb = reflectProb;
			sampleTransmit = false;
		}
		else
		{
			pathProb = 1.0_r - reflectProb;
			sampleReflect = false;
		}
	}

	math::Spectrum pdfAppliedBsdfCos{};
	math::Vector3R L{};
	if(sampleReflect)
	{
		// Calculate reflected L
		L = in.getV().mul(-1.0_r).reflect(N).normalizeLocal();
		if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		pdfAppliedBsdfCos = infR / pathProb;
	}
	else
	{
		PH_ASSERT(sampleTransmit);

		// Calculate transmitted L
		L = -in.getV();
		if(!ctx.sidedness.isOppositeHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		// Infinite bounce transmittance: TAT + TARARAT + TARARARARAT + ...
		math::Spectrum infT = TAT / (1.0_r - RA2);

		// Any TIR would result in a 0 term
		if(!infT.isFinite())
		{
			infT = math::Spectrum(0);
		}

		pdfAppliedBsdfCos = infT / pathProb;
	}

	out.setPdfAppliedBsdfCos(pdfAppliedBsdfCos, N.absDot(L));
	out.setL(L);
}

void ThinDielectricShell::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});
}

}// end namespace ph
