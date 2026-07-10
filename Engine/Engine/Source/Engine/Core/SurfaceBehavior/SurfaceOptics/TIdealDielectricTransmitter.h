#pragma once

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

#include <cmath>
#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

template<typename TransmissionScale>
class TIdealDielectricTransmitter : public SurfaceOptics
{
	static_assert(CSurfaceProperty<TransmissionScale, math::Spectrum>,
		"TransmissionScale must accept SurfaceHit and return a spectrum-convertible value.");

public:
	explicit TIdealDielectricTransmitter(std::shared_ptr<DielectricFresnel> fresnel)
		requires std::constructible_from<TransmissionScale, math::Spectrum>

		: TIdealDielectricTransmitter(
			std::move(fresnel),
			TransmissionScale(math::Spectrum(1)))
	{}

	TIdealDielectricTransmitter(
		std::shared_ptr<DielectricFresnel> fresnel,
		TransmissionScale                  transmissionScale)

		: SurfaceOptics()

		, m_fresnel          (std::move(fresnel))
		, m_transmissionScale(std::move(transmissionScale))
	{
		PH_ASSERT(m_fresnel);

		m_phenomena.set(ESurfacePhenomenon::DeltaTransmission);
	}

	ESurfacePhenomenon getPhenomenonOf(const SurfaceElemental elemental) const override
	{
		PH_ASSERT_EQ(elemental, 0);

		return ESurfacePhenomenon::DeltaTransmission;
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
		SampleFlow&             /* sampleFlow */,
		BsdfSampleOutput&       out) const override
	{
		const math::Vector3R N = in.getX().getShadingNormal();

		const auto optRefractDir = m_fresnel->calcRefractDir(in.getV(), N);
		if(!optRefractDir)
		{
			out.setContributability(false);
			return;
		}

		const math::Vector3R L = *optRefractDir;
		const real cosI = N.dot(L);

		math::Spectrum F = m_fresnel->calcTransmittance(in.getX(), cosI);

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
		F *= m_transmissionScale(in.getX());

		out.setPdfAppliedBsdfCos(F, std::abs(cosI));
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
	std::shared_ptr<DielectricFresnel> m_fresnel;

	[[PH_NO_UNIQUE_ADDRESS]]
	TransmissionScale m_transmissionScale;
};

}// end namespace ph
