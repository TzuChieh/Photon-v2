#pragma once

#include "Common/memory.h"

#include <cstdint>
#include <version>
#include <algorithm>
#include <type_traits>
#include <bit>
#include <cstring>
#include <new>

namespace ph
{

template<typename T>
inline auto make_aligned_memory(const std::size_t numBytes, const std::size_t alignmentInBytes)
-> TAlignedMemoryUniquePtr<T>
{
	if constexpr(!std::is_same_v<T, void>)
	{
		PH_ASSERT_EQ(alignmentInBytes % alignof(T), 0);
	}

	void* const ptr = detail::allocate_aligned_memory(numBytes, alignmentInBytes);

	// `static_cast` to `T*` is fine here: array types have implicit-lifetime, and now `ptr` points
	// to an array of `T` and pointer arithmetic is valid. Note that every element in `T*` still has
	// not started their lifetime if `T` is not an implicit-lifetime type.
	return TAlignedMemoryUniquePtr<T>(static_cast<T*>(ptr));
}

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

namespace detail
{

/*! Test whether `T` is an implicit-lifetime type. This concept is not exactly
`std::is_implicit_lifetime_v`, it is slightly permissive in the sense that true implicit-lifetime type
cannot have a user-provided destructor, and we are only testing if it is trivially destructible.
See P2674R0 "A trait for implicit lifetime types" for more details.
*/
template<typename T>
concept CPermissiveImplicitLifetime = std::disjunction_v<
	std::is_scalar<T>,
	std::is_array<T>,
	std::is_aggregate<T>,
	std::conjunction<
		std::is_trivially_destructible<T>,
		std::disjunction<
			std::is_trivially_default_constructible<T>,
			std::is_trivially_copy_constructible<T>,
			std::is_trivially_move_constructible<T>>>>;

}// end namespace detail

template<typename T>
inline T* start_implicit_lifetime_as(void* ptr) noexcept
{
#if __cpp_lib_start_lifetime_as
	return std::start_lifetime_as<T>(ptr);
#else
	return start_implicit_lifetime_as_array<T>(ptr, 1);
#endif
}

template<typename T>
inline T* start_implicit_lifetime_as_array(void* ptr, std::size_t numArrayElements) noexcept
{
	static_assert(
		std::is_trivially_copyable_v<T>
#if __cpp_lib_is_implicit_lifetime
		&& std::is_implicit_lifetime_v<T>
#else
		&& detail::CPermissiveImplicitLifetime<T>
#endif
		);

#if __cpp_lib_start_lifetime_as
	return std::start_lifetime_as_array<T>(ptr, numArrayElements);
#else
	// `std::memmove()` is one of the "magic" operations that implicitly create objects of
	// implicit lifetime type, we can hijack this property to do our work
	// (see https://stackoverflow.com/questions/76445860/implementation-of-stdstart-lifetime-as)
	// Note that using `new(ptr) std::byte[sizeof(T) * numArrayElements]` as in Robert Leahy's talk in
	// CppCon 2022 is an alternative to `std::memmove`, except that the object representation (value)
	// will be indeterminate due to placement new.
	return std::launder(static_cast<T*>(std::memmove(ptr, ptr, sizeof(T) * numArrayElements)));
#endif
}

}// end namespace ph
