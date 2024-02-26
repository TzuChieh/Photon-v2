#pragma once

#include "Core/Emitter/Sampler/EmitterSampler.h"

#include <vector>

namespace ph
{

class ESUniformRandom : public EmitterSampler
{
public:
	void update(TSpanView<const Emitter*> emitters) override;
	const Emitter* pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectSamplePdfW(
		DirectEnergySamplePdfQuery& query,
		HitProbe& probe) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph
