#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class OrenNayar : public SurfaceOptics
{
public:
	OrenNayar(
		const std::shared_ptr<TTexture<math::Spectrum>>& albedo,
		real sigmaDegrees);

	OrenNayar(
		const std::shared_ptr<TTexture<math::Spectrum>>& albedo,
		const std::shared_ptr<TTexture<real>>&           sigmaDegrees);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	std::string toString() const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfSamplePdfW(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	std::shared_ptr<TTexture<math::Spectrum>> m_albedo;
	std::shared_ptr<TTexture<real>>           m_sigmaDegrees;
};

// In-header Implementations:

inline std::string OrenNayar::toString() const
{
	return "Oren Nayar, " + SurfaceOptics::toString();
}

}// end namespace ph
