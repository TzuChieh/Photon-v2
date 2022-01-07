#pragma once

#include "Utility/IMoveOnly.h"
#include "Frame/TFrame.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Math/Function/TConstant2D.h"
#include "Math/math.h"

#include <cstddef>
#include <future>
#include <vector>
#include <algorithm>
#include <memory>

namespace ph
{

class mipmapgen final : private IMoveOnly
{
public:
	mipmapgen(const std::size_t numThreads);
	~mipmapgen()
	{
		m_workers.waitAllWorks();
	}

	// Generates a series of MIP levels from specified source frame.
	// Mipmaps are stored in the returned vector, where level 0 is source frame.
	// Each level is in half dimension (floored) of its previous level, and the 
	// last one is guaranteed to be 1x1.

	template<typename T, std::size_t N>
	using Mipmaps = std::vector<TFrame<T, N>>;

	template<typename T, std::size_t N>
	inline auto genMipmaps(const TFrame<T, N>& source) 
		-> std::future<Mipmaps<T, N>>
	{
		// Using shared_ptr here because if we move a std::promise to some lambda 
		// and use it to construct a Work, which is a std::function, then we can
		// not satisfy the CopyConstructible requirement that a std::function 
		// needs.
		auto promisedMipmaps = std::make_shared<std::promise<Mipmaps<T, N>>>();

		std::future<Mipmaps<T, N>> futureMipmaps = promisedMipmaps->get_future();

		m_workers.queueWork([workingResult = promisedMipmaps, src = source]()
		{
			TFrame<T, N> level0;
			if(math::is_power_of_2(src.widthPx()) && math::is_power_of_2(src.heightPx()))
			{
				level0 = src;
			}
			else
			{
				const uint32 newWidthPx  = math::next_power_of_2(src.widthPx());
				const uint32 newHeightPx = math::next_power_of_2(src.heightPx());
				level0 = TFrame<T, N>(newWidthPx, newHeightPx);

				src.sample(level0, math::TConstant2D<float64>(1.0), 2);
			}

			const std::size_t numMipmapLevels = 1 + math::log2_floor(std::max(level0.widthPx(), level0.heightPx()));
			Mipmaps<T, N> mipmaps(numMipmapLevels);
			mipmaps[0] = level0;
			for(std::size_t level = 1; level < numMipmapLevels; ++level)
			{
				auto& previous = mipmaps[level - 1];
				auto& current  = mipmaps[level];

				current = TFrame<T, N>(std::max(static_cast<uint32>(1), previous.widthPx() / 2), 
				                       std::max(static_cast<uint32>(1), previous.heightPx() / 2));
				previous.sample(current, math::TConstant2D<float64>(1.0), 2);
			}

			workingResult->set_value(std::move(mipmaps));
		});

		return std::move(futureMipmaps);
	}

private:
	FixedSizeThreadPool m_workers;
};

}// end namespace ph
