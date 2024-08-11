#pragma once

#include "Common/config.h"

#include <string>
#include <type_traits>
#include <array>
#include <cstddef>
#include <concepts>

namespace ph::detail
{

void output_not_implemented_warning(
	const std::string& filename,
	const std::string& lineNumber);

}// end namespace ph::detail

#define PH_NOT_IMPLEMENTED_WARNING()\
	do\
	{\
		::ph::detail::output_not_implemented_warning(\
			std::string(__FILE__),\
			std::to_string(__LINE__));\
	} while(0)

namespace ph
{

/*! @brief Calculates number of bits an instance of type `T` occupies.
*/
template<typename T>
consteval std::size_t sizeof_in_bits();

/*! @brief Creates an `std::array` filled with the same element.
@note The element does not need to be default-constructible.
*/
template<typename T, std::size_t N>
constexpr std::array<T, N> make_array(const T& element);

template<std::integral DstType, std::integral SrcType>
DstType lossless_integer_cast(const SrcType& src);

template<std::floating_point DstType, std::floating_point SrcType>
DstType lossless_float_cast(const SrcType& src);

/*! @brief Cast numeric value to another type without any loss of information.
If there is any possible overflow or numeric precision loss, exception is thrown.
@exception OverflowException If overflow happens.
@exception Numericxception If any numeric precision loss happens.
*/
template<typename DstType, typename SrcType>
DstType lossless_cast(const SrcType& src);

template<typename DstType, typename SrcType>
DstType lossless_cast(const SrcType& src, DstType* const out_dst);

}// end namespace ph

#include "Common/utility.ipp"
