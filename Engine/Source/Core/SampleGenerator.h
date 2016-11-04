#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"

#include <vector>

namespace ph
{

class Frame;

class SampleGenerator
{
public:
	virtual ~SampleGenerator() = 0;

	virtual bool hasMoreSamples() const = 0;
	virtual void requestMoreSamples(const Frame& frame, std::vector<Sample>* out_samples) = 0;
};

}// end namespace ph