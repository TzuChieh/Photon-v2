#pragma once

#include "Core/Emitter/Sampler/EmitterSampler.h"

#include <vector>

namespace ph
{

class ESUniformRandom : public EmitterSampler
{
public:
	void update(const CookedDataStorage& cookedActors) override;
	const Emitter* pickEmitter(real* const out_PDF) const override;
	void genDirectSample(DirectLightSample& sample) const override;
	real calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph