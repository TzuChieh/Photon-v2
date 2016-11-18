#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"

#include <vector>

namespace ph
{

class Film;

class SampleGenerator
{
public:
	SampleGenerator(const uint32 sppBudget);
	virtual ~SampleGenerator() = 0;

	virtual bool hasMoreSamples() const = 0;
	virtual void requestMoreSamples(const Film& film, std::vector<Sample>* const out_samples) = 0;

	inline uint32 getSppBudget() const
	{
		return m_sppBudget;
	}

private:
	uint32 m_sppBudget;
};

}// end namespace ph