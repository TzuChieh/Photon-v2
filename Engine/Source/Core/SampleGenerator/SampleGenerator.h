#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"

#include <vector>
#include <memory>

namespace ph
{

class Film;
class World;

class SampleGenerator
{
public:
	SampleGenerator(const uint32 sppBudget);
	virtual ~SampleGenerator() = 0;

	virtual bool hasMoreSamples() const = 0;
	virtual void analyze(const World& world, const Film& film) = 0;
	virtual void requestMoreSamples(std::vector<Sample>* const out_samples) = 0;
	virtual void split(const uint32 nSplits, std::vector<std::unique_ptr<SampleGenerator>>* const out_sampleGenerators) = 0;

	inline uint32 getSppBudget() const
	{
		return m_sppBudget;
	}

private:
	uint32 m_sppBudget;
};

}// end namespace ph