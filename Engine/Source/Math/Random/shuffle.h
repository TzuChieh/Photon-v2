#pragma once

#include "Common/assertion.h"
#include "Math/Random.h"

#include <cstddef>
#include <algorithm>
#include <iterator>

namespace ph::math
{

namespace shuffle_detail
{

template<typename T>
struct StandardSwapper
{
	void operator () (T& a, T& b)
	{
		// enable ADL
		using std::swap;

		swap(a, b);
	}
};

}// end namespace shuffle_detail

template<typename IndexPairConsumer>
void shuffle_durstenfeld_index_pairs(
	const std::size_t       beginIndex,
	const std::size_t       endIndex,
	const IndexPairConsumer consumer)
{
	PH_ASSERT_LE(beginIndex, endIndex);

	for(std::size_t i = beginIndex; i < endIndex; ++i)
	{
		const std::size_t ri = math::Random::genUniformIndex_iL_eU(i, endIndex);
		consumer(i, ri);
	}
}

template<
	typename RandomIterator, 
	typename Swapper = shuffle_detail::StandardSwapper<typename std::iterator_traits<RandomIterator>::value_type>>
void shuffle_durstenfeld(
	RandomIterator begin, RandomIterator end,
	Swapper swapper = Swapper())
{
	PH_ASSERT(begin <= end);
	const auto NUM_ELEMENTS = static_cast<std::size_t>(std::distance(begin, end));

	shuffle_durstenfeld_index_pairs(
		0, NUM_ELEMENTS, 
		[=](const std::size_t indexA, const std::size_t indexB)
		{
			swapper(begin[indexA], begin[indexB]);
		});
}

}// end namespace ph::math
