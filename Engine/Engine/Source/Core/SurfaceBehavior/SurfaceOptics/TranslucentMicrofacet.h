#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"

#include <memory>

namespace ph
{

/*! @brief Microfacet based translucent surface model.
Microfacet optical behavior modeled using the dielectric variant @cite Walter:2007:Microfacet
of the Cook-Torrance BRDF @cite Cook:1981:Reflectance.
This model is double-sided.
*/
class TranslucentMicrofacet : public SurfaceOptics
{
public:
	TranslucentMicrofacet(
		const std::shared_ptr<DielectricFresnel>& fresnel, 
		const std::shared_ptr<Microfacet>&        microfacet);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	std::string toString() const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	std::shared_ptr<DielectricFresnel> m_fresnel;
	std::shared_ptr<Microfacet>        m_microfacet;

	static real getReflectionProbability(const math::Spectrum& F);

	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;
};

// In-header Implementations:

inline std::string TranslucentMicrofacet::toString() const
{
	return "Translucent Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph
