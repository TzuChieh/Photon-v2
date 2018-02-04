#pragma once

#include "Utility/INoncopyable.h"
#include "Frame/TFrame.h"

#include <cstddef>
#include <future>
#include <vector>

namespace ph
{

class FrameProcessor final : public INoncopyable
{
public:
	FrameProcessor(const std::size_t maxThreads);
	~FrameProcessor() = default;

	// Generates a series of MIP levels from specified source frame.
	// Mipmaps are stored in the returned vector, where level 0 is source frame.
	// Each level is in half dimension (floored) of its previous level, and the 
	// last one is guaranteed to be 1x1.
	/*template<typename FrameComponent, std::size_t NUM_FRAME_COMPONENTS>
	inline auto genMipmaps(const TFrame<FrameComponent, NUM_FRAME_COMPONENTS>& source) 
		-> std::future<std::vector<TFrame<FrameComponent, NUM_FRAME_COMPONENTS>>>
	{

	}*/

private:
	std::size_t m_maxThreads;
};

}// end namespace ph