#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

/*! @brief Lambertian diffuse model for an opaque surface.
This model is double-sided.
*/
class LambertianReflector : public SurfaceOptics
{
public:
	explicit LambertianReflector(const std::shared_ptr<TTexture<math::Spectrum>>& albedo);

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
	std::shared_ptr<TTexture<math::Spectrum>> m_albedo;
};

// In-header Implementations:

inline std::string LambertianReflector::toString() const
{
	return "Lambertian Diffuse, " + SurfaceOptics::toString();
}

}// end namespace ph
