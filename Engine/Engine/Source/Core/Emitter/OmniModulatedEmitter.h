#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Core/Intersection/UvwMapper/SphericalMapper.h"

#include <Common/primitive_type.h>

namespace ph
{

// TODO: importance sampling based on filter values

class OmniModulatedEmitter : public Emitter
{
public:
	explicit OmniModulatedEmitter(
		const Emitter*    source,
		EmitterFeatureSet featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* out_energy) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override;

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
