#pragma once

#include "Common/memory.h"

#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <bit>

namespace ph
{

template<typename T>
inline void from_bytes(const std::byte* const srcBytes, T* const out_dstValue)
{
	static_assert(std::is_trivially_copyable_v<T>);

	PH_ASSERT(srcBytes);
	PH_ASSERT(out_dstValue);

	std::copy(
		srcBytes,
		srcBytes + sizeof(T),
		reinterpret_cast<std::byte*>(out_dstValue));
}

template<typename T>
inline void to_bytes(const T& srcValue, std::byte* const out_dstBytes)
{
	static_assert(std::is_trivially_copyable_v<T>);

	PH_ASSERT(out_dstBytes);

	std::copy(
		reinterpret_cast<const std::byte*>(&srcValue),
		reinterpret_cast<const std::byte*>(&srcValue) + sizeof(T),
		out_dstBytes);
}

template<std::size_t N>
inline void reverse_bytes(std::byte* const bytes)
{
	PH_ASSERT(bytes);

	if constexpr(N == 1)
	{
		// Nothing to do, single byte is already its own reverse
		return;
	}
	else if constexpr(N == 2)
	{
		std::uint16_t twoBytes;
		from_bytes(bytes, &twoBytes);
		twoBytes = std::byteswap(twoBytes);
		to_bytes(twoBytes, bytes);
	}
	else if constexpr(N == 4)
	{
		std::uint32_t fourBytes;
		from_bytes(bytes, &fourBytes);
		fourBytes = std::byteswap(fourBytes);
		to_bytes(fourBytes, bytes);
	}
	else if constexpr(N == 8)
	{
		std::uint64_t eightBytes;
		from_bytes(bytes, &eightBytes);
		eightBytes = std::byteswap(eightBytes);
		to_bytes(eightBytes, bytes);
	}
	else
	{
		// Invalid `N` (number of bytes)
		// (one may also consider to add impl for `N`)
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph
