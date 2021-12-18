#pragma once

#include "Core/Emitter/Emitter.h"
#include "Common/primitive_type.h"
#include "Core/Texture/TTexture.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"

#include <memory>

namespace ph
{

// TODO: importance sampling based on filter values

class OmniModulatedEmitter final : public Emitter
{
public:
	OmniModulatedEmitter(std::unique_ptr<Emitter> source);

	void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const override;
	void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const override;

	// FIXME: ray time
	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const override;
	real calcRadiantFluxApprox() const override;

	void setFilter(const std::shared_ptr<TTexture<math::Spectrum>>& filter);

private:
	std::unique_ptr<Emitter>                  m_source;
	std::shared_ptr<TTexture<math::Spectrum>> m_filter;
	SphericalMapper                           m_dirToUv;
};

}// end namespace ph
