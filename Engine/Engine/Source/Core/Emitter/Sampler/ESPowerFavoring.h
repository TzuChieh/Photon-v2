#pragma once

#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Math/Random/TPwcDistribution1D.h"

#include <vector>
#include <unordered_map>

namespace ph
{

class ESPowerFavoring : public EmitterSampler
{
public:
	void update(TSpanView<const Emitter*> emitters) override;
	const Emitter* pickEmitter(SampleFlow& sampleFlow, real* out_PDF) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override;

private:
	std::vector<const Emitter*>    m_emitters;
	math::TPwcDistribution1D<real> m_distribution;

	std::unordered_map<
		const Emitter*, std::size_t
	> m_emitterToIndexMap;
};

}// end namespace ph
