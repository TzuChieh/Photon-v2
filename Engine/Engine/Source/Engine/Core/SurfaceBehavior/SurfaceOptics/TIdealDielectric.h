#pragma once

#include "Engine/Core/SampleGenerator/SampleFlow.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

template<typename ReflectionScale, typename TransmissionScale>
class TIdealDielectric : public SurfaceOptics
{
	static_assert(CSurfaceProperty<ReflectionScale, math::Spectrum>,
		"ReflectionScale must accept SurfaceHit and return a spectrum-convertible value.");
	static_assert(CSurfaceProperty<TransmissionScale, math::Spectrum>,
		"TransmissionScale must accept SurfaceHit and return a spectrum-convertible value.");

public:
	explicit TIdealDielectric(std::shared_ptr<DielectricFresnel> fresnel)
		requires std::constructible_from<ReflectionScale, math::Spectrum> &&
		         std::constructible_from<TransmissionScale, math::Spectrum>

		: TIdealDielectric(
			std::move(fresnel),
			ReflectionScale(math::Spectrum(1)),
			TransmissionScale(math::Spectrum(1)))
	{}

	TIdealDielectric(
		std::shared_ptr<DielectricFresnel> fresnel,
		ReflectionScale                    reflectionScale,
		TransmissionScale                  transmissionScale)

		: SurfaceOptics()

		, m_fresnel          (std::move(fresnel))
		, m_reflectionScale  (std::move(reflectionScale))
		, m_transmissionScale(std::move(transmissionScale))
	{
		PH_ASSERT(m_fresnel);

		m_phenomena.set({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
		m_numElementals = 2;
	}

	ESurfacePhenomenon getPhenomenonOf(const SurfaceElemental elemental) const override
	{
		PH_ASSERT_IN_RANGE(elemental, 0, 2);

		return elemental == REFLECTION ? ESurfacePhenomenon::DeltaReflection :
		                                 ESurfacePhenomenon::DeltaTransmission;
	}

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override
	{
		out.setContributability(false);
	}

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override
	{
		const bool canReflect  = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION;
		const bool canTransmit = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION;

		if(!canReflect && !canTransmit)
		{
			out.setContributability(false);
			return;
		}

		const math::Vector3R N = in.getX().getShadingNormal();

		math::Spectrum F = m_fresnel->calcReflectance(in.getX(), N.dot(in.getV()));
		const real reflectProb = F.avg();

		bool sampleReflect  = canReflect;
		bool sampleTransmit = canTransmit;

		// We cannot sample both paths, choose one stochastically
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
				out.setContributability(false);
				return;
			}

			// A scale factor for artistic control
			F.mulLocal(m_reflectionScale(in.getX()));

			// Account for pick probability
			if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
			{
				F.divLocal(reflectProb);
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
			F = m_fresnel->calcTransmittance(in.getX(), N.dot(L));

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
			F.mulLocal(m_transmissionScale(in.getX()));

			// Account for pick probability
			if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
			{
				F.divLocal(1.0_r - reflectProb);
			}
		}
		else
		{
			// RARE: may be called due to numerical error
			out.setContributability(false);
			return;
		}

		out.setPdfAppliedBsdfCos(F, N.absDot(L));
		out.setL(L);
	}

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override
	{
		out.setSampleDirPdf({});
	}

private:
	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;

	std::shared_ptr<DielectricFresnel> m_fresnel;

	[[PH_NO_UNIQUE_ADDRESS]]
	ReflectionScale m_reflectionScale;

	[[PH_NO_UNIQUE_ADDRESS]]
	TransmissionScale m_transmissionScale;
};

}// end namespace ph
