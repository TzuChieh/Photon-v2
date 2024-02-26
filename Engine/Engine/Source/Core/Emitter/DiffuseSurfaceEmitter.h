#pragma once

#include "Core/Emitter/SurfaceEmitter.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class Primitive;

class DiffuseSurfaceEmitter : public SurfaceEmitter
{
public:
	explicit DiffuseSurfaceEmitter(const Primitive* surface);

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

	const Primitive* getSurface() const;
	void setEmittedRadiance(const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance);
	const TTexture<math::Spectrum>& getEmittedRadiance() const;

private:
	const Primitive* m_surface;
	real m_reciExtendedArea;
	std::shared_ptr<TTexture<math::Spectrum>> m_emittedRadiance;
};

}// end namespace ph
