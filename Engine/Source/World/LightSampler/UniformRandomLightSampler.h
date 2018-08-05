#pragma once

#include "World/LightSampler/LightSampler.h"

#include <vector>

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const CookedDataStorage& cookedActors) override;
	virtual const Emitter* pickEmitter(real* const out_PDF) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;
	virtual real calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph