#pragma once

#include "Common/utility.h"
#include "Common/assertion.h"
#include "Common/exceptions.h"

#include <utility>
#include <climits>
#include <limits>

namespace ph
{

namespace detail
{

template<typename T, std::size_t... Is>
inline constexpr std::array<T, sizeof...(Is)> make_array(
	T element,
	std::index_sequence<Is...>)
{
	// Use sequence as pattern to repeat `element`, also to avoid unused warnings
	return {(static_cast<void>(Is), element)...};
}

}// end namespace detail

template<typename T>
inline consteval std::size_t sizeof_in_bits()
{
	return CHAR_BIT * sizeof(T);
}

template<typename T, std::size_t N>
inline constexpr std::array<T, N> make_array(const T& element)
{
	return detail::make_array(element, std::make_index_sequence<N>());
}

template<std::integral DstType, std::integral SrcType>
inline DstType lossless_integer_cast(const SrcType& src)
{
	using SrcLimits = std::numeric_limits<SrcType>;
	using DstLimits = std::numeric_limits<DstType>;

	// Note that the use of `std::cmp_<X>` functions are important as the comparisons 
	// may be signed <-> unsigned comparisons, which may cause signed limits to overflow

	// TODO: we may need to cast src to some integer first to support char and bool types (they are not supported by cmp functions)

	constexpr bool mayHavePositiveOverflow = std::cmp_greater(SrcLimits::max(), DstLimits::max());
	constexpr bool mayHaveNegativeOverflow = std::cmp_less(SrcLimits::lowest(), DstLimits::lowest());

	if constexpr(mayHavePositiveOverflow)
	{
		if(std::cmp_greater(src, DstLimits::max()))
		{
			throw_formatted<OverflowException>("cast results in positive overflow: {} exceeds the limit {}",
				src, DstLimits::max());
		}
	}

	if constexpr(mayHaveNegativeOverflow)
	{
		if(std::cmp_less(src, DstLimits::lowest()))
		{
			throw_formatted<OverflowException>("cast results in negative overflow: {} exceeds the limit {}",
				src, DstLimits::lowest());
		}
	}

	// All possible integer overflow scenarios are checked so it is safe to cast now
	return static_cast<DstType>(src);
}

template<std::floating_point DstType, std::floating_point SrcType>
inline DstType lossless_float_cast(const SrcType& src)
{
	// Nothing to do if both types are the same
	if constexpr(std::is_same_v<SrcType, DstType>)
	{
		return src;
	}
	// If we are converting to a wider floating-point type, generally it will be lossless
	else if constexpr(sizeof(DstType) > sizeof(SrcType))
	{
		// We need both types to be IEEE-754
		static_assert(std::numeric_limits<SrcType>::is_iec559);
		static_assert(std::numeric_limits<DstType>::is_iec559);

		return static_cast<DstType>(src);
	}
	// Otherwise, cast to `DstType` then back to `SrcType` and see if there is any difference
	else
	{
		const auto dst = static_cast<DstType>(src);
		const auto dstBackToSrc = static_cast<SrcType>(dst);
		if(src != dstBackToSrc)
		{
			throw_formatted<NumericException>("cast results in numeric precision loss: {} -> {}",
				src, dstBackToSrc);
		}

		return dst;
	}
}

template<typename DstType, typename SrcType>
inline DstType lossless_cast(const SrcType& src)
{
	// Integer -> Integer
	if constexpr(std::is_integral_v<SrcType> && std::is_integral_v<DstType>)
	{
		return lossless_integer_cast<DstType>(src);
	}
	// Integer -> Floating-point
	else if constexpr(std::is_integral_v<SrcType> && std::is_floating_point_v<DstType>)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
	// Floating-point -> Integer
	else if constexpr(std::is_floating_point_v<SrcType> && std::is_integral_v<DstType>)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
	// Floating-point -> Floating-point
	else
	{
		static_assert(std::is_floating_point_v<SrcType> && std::is_floating_point_v<DstType>);

		return lossless_float_cast<DstType>(src);
	}
}

template<typename DstType, typename SrcType>
inline DstType lossless_cast(const SrcType& src, DstType* const out_dst)
{
	PH_ASSERT(out_dst);

	*out_dst = lossless_cast<DstType>(src);
	return *out_dst;
}

}// end namespace ph
