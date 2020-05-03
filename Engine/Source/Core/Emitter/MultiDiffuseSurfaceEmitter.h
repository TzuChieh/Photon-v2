#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"

#include <vector>

namespace ph
{

class MultiDiffuseSurfaceEmitter : public SurfaceEmitter
{
public:
	MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters);

	void evalEmittedRadiance(const SurfaceHit& X, Spectrum* out_radiance) const override;
	void genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const override;

	// FIXME: ray time
	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const override;
	real calcRadiantFluxApprox() const override;

	void setFrontFaceEmit() override;
	void setBackFaceEmit() override;

	void addEmitter(const DiffuseSurfaceEmitter& emitter);
	void setEmittedRadiance(const std::shared_ptr<TTexture<Spectrum>>& emittedRadiance);
	const TTexture<Spectrum>& getEmittedRadiance() const;

private:
	std::vector<DiffuseSurfaceEmitter> m_emitters;
	real                               m_extendedArea;
	real                               m_reciExtendedArea;
};

}// end namespace ph
