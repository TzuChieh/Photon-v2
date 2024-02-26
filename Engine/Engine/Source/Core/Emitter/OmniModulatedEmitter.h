#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Core/Intersection/UvwMapper/SphericalMapper.h"

#include <Common/primitive_type.h>

namespace ph
{

// TODO: importance sampling based on filter values

class OmniModulatedEmitter final : public Emitter
{
public:
	OmniModulatedEmitter(const Emitter* source);

	void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectSamplePdfW(
		DirectEnergySamplePdfQuery& query,
		HitProbe& probe) const override;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	real calcRadiantFluxApprox() const override;

	void setFilter(const std::shared_ptr<TTexture<math::Spectrum>>& filter);

private:
	const Emitter*                            m_source;
	std::shared_ptr<TTexture<math::Spectrum>> m_filter;
	SphericalMapper                           m_dirToUv;
};

}// end namespace ph
