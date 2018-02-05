#pragma once

#include "Utility/INoncopyable.h"
#include "Frame/TFrame.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Math/Function/TConstant2D.h"

#include <cstddef>
#include <future>
#include <vector>

namespace ph
{

class FrameProcessor final : public INoncopyable
{
public:
	FrameProcessor(const std::size_t numThreads);
	~FrameProcessor() = default;

	// Generates a series of MIP levels from specified source frame.
	// Mipmaps are stored in the returned vector, where level 0 is source frame.
	// Each level is in half dimension (floored) of its previous level, and the 
	// last one is guaranteed to be 1x1.

	template<typename T, std::size_t N>
	using Mipmaps = std::future<std::vector<TFrame<T, N>>>;

	template<typename T, std::size_t N>
	inline auto genMipmaps(const TFrame<T, N>& source) 
		-> std::future<Mipmaps<T, N>>
	{
		std::promise<Mipmaps<T, N>> result;
		std::future<Mipmaps<T, N>> mipmaps = result.get_future();

		m_workers.queueWork([workingResult = std::move(result)]()
		{
			//TFrame<T, N> level0;
			//if(source)
		});

		return mipmaps;
	}

private:
	FixedSizeThreadPool m_workers;
};

}// end namespace ph