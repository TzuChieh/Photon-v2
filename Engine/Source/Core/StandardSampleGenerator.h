#pragma once

#include "Core/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class StandardSampleGenerator final : public SampleGenerator
{
public:
	StandardSampleGenerator(const uint32 sppBudget);
	virtual ~StandardSampleGenerator() override;

	virtual bool hasMoreSamples() const override;
	virtual void requestMoreSamples(const Film& film, std::vector<Sample>* out_samples) override;

private:
	uint32 m_numDispatchedSpp;
};

}// end namespace ph