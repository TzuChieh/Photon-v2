#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"

#include <vector>

namespace ph
{

class MultiDiffuseSurfaceEmitter final : public SurfaceEmitter
{
public:
	MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters);
	virtual ~MultiDiffuseSurfaceEmitter() override;

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;

	// FIXME: ray time
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const override;
	virtual void setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance) override;

	virtual void setFrontFaceEmit() override;
	virtual void setBackFaceEmit() override;

	void addEmitter(const DiffuseSurfaceEmitter& emitter);

private:
	std::vector<DiffuseSurfaceEmitter>          m_emitters;
	real                                        m_extendedArea;
	real                                        m_reciExtendedArea;
};

}// end namespace ph