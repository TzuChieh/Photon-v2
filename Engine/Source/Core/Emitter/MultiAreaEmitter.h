#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"

#include <vector>

namespace ph
{

class MultiAreaEmitter final : public Emitter
{
public:
	MultiAreaEmitter(const std::vector<PrimitiveAreaEmitter>& areaEmitters);
	virtual ~MultiAreaEmitter() override;

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;

	// FIXME: ray time
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const override;

	virtual inline bool isSurfaceEmissive() const
	{
		return true;
	}

	inline void addEmitter(const PrimitiveAreaEmitter& emitter)
	{
		m_areaEmitters.push_back(emitter);
	}

	void setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance);

private:
	std::vector<PrimitiveAreaEmitter>           m_areaEmitters;
	std::shared_ptr<TTexture<SpectralStrength>> m_emittedRadiance;
	real                                        m_extendedArea;
	real                                        m_reciExtendedArea;
};

}// end namespace ph