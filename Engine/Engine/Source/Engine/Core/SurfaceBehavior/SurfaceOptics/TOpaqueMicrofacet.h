#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/Property/Microfacet.h"
#include "Engine/Core/SurfaceBehavior/Property/ConductorFresnel.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/compiler.h>

#include <concepts>
#include <memory>

namespace ph
{

/*! @brief Microfacet-based opaque surface model.
Microfacet optical behavior modeled using Cook-Torrance BRDF @cite Cook:1981:Reflectance.
This model is double-sided and single-scattering. Reflected energy may be scaled by an artistic
spectrum property.
*/
template<typename ReflectionScale>
class TOpaqueMicrofacet : public SurfaceOptics
{
	static_assert(CSurfaceProperty<ReflectionScale, math::Spectrum>,
		"ReflectionScale must accept SurfaceHit and return a spectrum-convertible value.");

public:
	TOpaqueMicrofacet(
		std::shared_ptr<ConductorFresnel> fresnel,
		std::shared_ptr<Microfacet>       microfacet)
		requires std::constructible_from<ReflectionScale, math::Spectrum>;

	TOpaqueMicrofacet(
		std::shared_ptr<ConductorFresnel> fresnel,
		std::shared_ptr<Microfacet>       microfacet,
		ReflectionScale                   reflectionScale);

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
	std::shared_ptr<Microfacet>       m_microfacet;
	std::shared_ptr<ConductorFresnel> m_fresnel;

	[[PH_NO_UNIQUE_ADDRESS]]
	ReflectionScale m_reflectionScale;
};

using OpaqueMicrofacet = TOpaqueMicrofacet<TConstantSurfaceProperty<math::Spectrum>>;

extern template class TOpaqueMicrofacet<TConstantSurfaceProperty<math::Spectrum>>;
extern template class TOpaqueMicrofacet<TTexturedSurfaceProperty<math::Spectrum>>;

// In-header Implementations:

template<typename ReflectionScale>
inline std::string TOpaqueMicrofacet<ReflectionScale>::toString() const
{
	return "Opaque Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph
