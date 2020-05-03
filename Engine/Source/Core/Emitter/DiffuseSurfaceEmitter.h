#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;

class DiffuseSurfaceEmitter : public SurfaceEmitter
{
public:
	DiffuseSurfaceEmitter(const Primitive* surface);

	void evalEmittedRadiance(const SurfaceHit& X, Spectrum* out_radiance) const override;
	void genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const override;
	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;
	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const override;
	real calcRadiantFluxApprox() const override;

	const Primitive* getSurface() const;
	void setEmittedRadiance(const std::shared_ptr<TTexture<Spectrum>>& emittedRadiance);
	const TTexture<Spectrum>& getEmittedRadiance() const;

private:
	const Primitive* m_surface;
	real m_reciExtendedArea;
	std::shared_ptr<TTexture<Spectrum>> m_emittedRadiance;
};

}// end namespace ph
