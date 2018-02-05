#include "Frame/FrameProcessor.h"

namespace ph
{

FrameProcessor::FrameProcessor(const std::size_t numThreads) : 
	m_workers(numThreads)
{

}

}// end namespace ph