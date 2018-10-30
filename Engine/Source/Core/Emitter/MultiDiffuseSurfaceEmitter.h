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

	void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	void genDirectSample(DirectLightSample& sample) const override;

	// FIXME: ray time
	void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;
	real calcRadiantFluxApprox() const override;

	void setFrontFaceEmit() override;
	void setBackFaceEmit() override;

	void addEmitter(const DiffuseSurfaceEmitter& emitter);
	void setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance);
	const TTexture<SpectralStrength>& getEmittedRadiance() const;

private:
	std::vector<DiffuseSurfaceEmitter> m_emitters;
	real                               m_extendedArea;
	real                               m_reciExtendedArea;
};

}// end namespace ph