#pragma once

#include "World/LightSampler/LightSampler.h"

#include <vector>

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const CookedActorStorage& cookedActors) override;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;
	virtual float32 calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph