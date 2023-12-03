#pragma once

#include <cstddef>

namespace ph
{

class SampleContext final
{
public:
	std::size_t sampleBatchIndex;

	explicit SampleContext(std::size_t sampleBatchIndex);
};

// In-header Implementations:

inline SampleContext::SampleContext(const std::size_t sampleBatchIndex) :
	sampleBatchIndex(sampleBatchIndex)
{}

}// namespace ph
