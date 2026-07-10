#pragma once

#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/Property/FresnelEffect.h"
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

template<typename ReflectionScale>
class TIdealReflector : public SurfaceOptics
{
	static_assert(CSurfaceProperty<ReflectionScale, math::Spectrum>,
		"ReflectionScale must accept SurfaceHit and return a spectrum-convertible value.");

public:
	explicit TIdealReflector(std::shared_ptr<FresnelEffect> fresnel)
		requires std::constructible_from<ReflectionScale, math::Spectrum>

		: TIdealReflector(
			std::move(fresnel),
			ReflectionScale(math::Spectrum(1)))
	{}

	TIdealReflector(
		std::shared_ptr<FresnelEffect> fresnel,
		ReflectionScale                reflectionScale)

		: SurfaceOptics()

		, m_fresnel        (std::move(fresnel))
		, m_reflectionScale(std::move(reflectionScale))
	{
		PH_ASSERT(m_fresnel);

		m_phenomena.set(ESurfacePhenomenon::DeltaReflection);
	}

	ESurfacePhenomenon getPhenomenonOf(const SurfaceElemental elemental) const override
	{
		PH_ASSERT_EQ(elemental, 0);

		return ESurfacePhenomenon::DeltaReflection;
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
		const math::Vector3R L = in.getV().mul(-1.0_r).reflect(N);
		const real NoL = N.dot(L);

		// A scale factor for artistic control
		math::Spectrum F = m_fresnel->calcReflectance(in.getX(), NoL);
		F *= m_reflectionScale(in.getX());

		out.setPdfAppliedBsdfCos(F, std::abs(NoL));
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
	std::shared_ptr<FresnelEffect> m_fresnel;

	[[PH_NO_UNIQUE_ADDRESS]]
	ReflectionScale m_reflectionScale;
};

}// end namespace ph
