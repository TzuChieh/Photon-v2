#pragma once

#include "Math/Random/Random.h"

#include <Common/assertion.h>

#include <cstddef>
#include <algorithm>
#include <iterator>
#include <utility>

namespace ph::math
{

namespace detail::shuffle
{

template<typename T>
struct StandardSwapper
{
	void operator () (T& a, T& b) const
	{
		// enable ADL
		using std::swap;

		swap(a, b);
	}
};

}// end namespace detail::shuffle

template<typename IndexPairConsumer>
void shuffle_durstenfeld_index_pairs(
	const std::size_t       beginIndex,
	const std::size_t       endIndex,
	IndexPairConsumer       consumer)
{
	PH_ASSERT_LE(beginIndex, endIndex);

	for(std::size_t i = beginIndex; i < endIndex; ++i)
	{
		const std::size_t ri = math::Random::index(i, endIndex);
		consumer(i, ri);
	}
}

template<
	typename RandomIterator, 
	typename Swapper = detail::shuffle::StandardSwapper<typename std::iterator_traits<RandomIterator>::value_type>>
void shuffle_durstenfeld(
	RandomIterator begin, RandomIterator end,
	Swapper swapper = Swapper())
{
	PH_ASSERT(begin <= end);
	const auto NUM_ELEMENTS = static_cast<std::size_t>(std::distance(begin, end));

	shuffle_durstenfeld_index_pairs(
		0, NUM_ELEMENTS, 
		[=, &swapper](
			const std::size_t indexA, const std::size_t indexB)
		{
			swapper(begin[indexA], begin[indexB]);
		});
}

}// end namespace ph::math
