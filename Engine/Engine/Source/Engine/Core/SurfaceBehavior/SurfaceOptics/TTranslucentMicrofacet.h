#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/Microfacet.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/compiler.h>

#include <concepts>
#include <memory>

namespace ph
{

/*! @brief Microfacet-based translucent surface model.
Microfacet optical behavior modeled using the dielectric variant @cite Walter:2007:Microfacet
of the Cook-Torrance BRDF @cite Cook:1981:Reflectance.
This model is double-sided. Reflected and transmitted energy may be scaled independently by
a spectral artistic control.
*/
template<typename ReflectionScale, typename TransmissionScale>
class TTranslucentMicrofacet : public SurfaceOptics
{
	static_assert(CSurfaceProperty<ReflectionScale, math::Spectrum>,
		"ReflectionScale must accept SurfaceHit and return a spectrum-convertible value.");
	static_assert(CSurfaceProperty<TransmissionScale, math::Spectrum>,
		"TransmissionScale must accept SurfaceHit and return a spectrum-convertible value.");

public:
	TTranslucentMicrofacet(
		std::shared_ptr<DielectricFresnel> fresnel,
		std::shared_ptr<Microfacet>        microfacet)
		requires std::constructible_from<ReflectionScale, math::Spectrum> &&
		         std::constructible_from<TransmissionScale, math::Spectrum>;

	TTranslucentMicrofacet(
		std::shared_ptr<DielectricFresnel> fresnel,
		std::shared_ptr<Microfacet>        microfacet,
		ReflectionScale                    reflectionScale,
		TransmissionScale                  transmissionScale);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::string toString() const override;

private:
	std::shared_ptr<DielectricFresnel> m_fresnel;
	std::shared_ptr<Microfacet>        m_microfacet;

	[[PH_NO_UNIQUE_ADDRESS]]
	ReflectionScale m_reflectionScale;

	[[PH_NO_UNIQUE_ADDRESS]]
	TransmissionScale m_transmissionScale;

	static real getReflectionProbability(const math::Spectrum& F);

	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;
};

using TranslucentMicrofacet = TTranslucentMicrofacet<
	TConstantSurfaceProperty<math::Spectrum>,
	TConstantSurfaceProperty<math::Spectrum>>;

extern template class TTranslucentMicrofacet<
	TConstantSurfaceProperty<math::Spectrum>,
	TConstantSurfaceProperty<math::Spectrum>>;
extern template class TTranslucentMicrofacet<
	TConstantSurfaceProperty<math::Spectrum>,
	TTexturedSurfaceProperty<math::Spectrum>>;
extern template class TTranslucentMicrofacet<
	TTexturedSurfaceProperty<math::Spectrum>,
	TConstantSurfaceProperty<math::Spectrum>>;
extern template class TTranslucentMicrofacet<
	TTexturedSurfaceProperty<math::Spectrum>,
	TTexturedSurfaceProperty<math::Spectrum>>;

// In-header Implementations:

template<typename ReflectionScale, typename TransmissionScale>
inline std::string TTranslucentMicrofacet<ReflectionScale, TransmissionScale>::toString() const
{
	return "Translucent Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph
