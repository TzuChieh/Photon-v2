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
	FrameProcessor() = default;
	~FrameProcessor() = default;

	// Generates a series of MIP levels from specified source frame.
	// Mipmaps are stored in the returned vector, where level 0 is source frame
	// and level
	template<typename FrameComponent, std::size_t NUM_FRAME_COMPONENTS>
	inline auto genMipmaps(const TFrame<FrameComponent, NUM_FRAME_COMPONENTS>& source) 
		-> std::future<std::vector<TFrame<FrameComponent, NUM_FRAME_COMPONENTS>>>
	{

	}

private:
};

}// end namespace ph