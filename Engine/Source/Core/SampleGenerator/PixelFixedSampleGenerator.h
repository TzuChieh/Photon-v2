#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class PixelFixedSampleGenerator final : public SampleGenerator
{
public:
	PixelFixedSampleGenerator(const uint32 sppBudget);
	virtual ~PixelFixedSampleGenerator() override;

	virtual bool hasMoreSamples() const override;
	virtual void analyze(const World& world, const Film& film) override;
	virtual void requestMoreSamples(std::vector<Sample>* const out_samples) override;
	virtual void split(const uint32 nSplits, std::vector<std::unique_ptr<SampleGenerator>>* const out_sampleGenerators) override;

private:
	uint32 m_numDispatchedSpp;
	uint32 m_filmWidthPx;
	uint32 m_filmHeightPx;

	bool canSplit(const uint32 nSplits) const;
};

}// end namespace ph