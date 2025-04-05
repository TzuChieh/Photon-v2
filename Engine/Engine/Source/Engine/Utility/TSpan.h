#pragma once

#include <span>

namespace ph
{

/*! @brief @brief A contiguous sequence of objects of type `T`.
Effectively the same as `std::span`.
*/
template<typename T, std::size_t EXTENT = std::dynamic_extent>
using TSpan = std::span<T, EXTENT>;

/*! @brief Same as `TSpan`, except that the objects are const-qualified.
Note that for pointer types, the const is applied on the pointer itself. For example,
when `T` is `const int*`, the accessed type will be `const int* const`.
*/
template<typename T, std::size_t EXTENT = std::dynamic_extent>
using TSpanView = std::span<const T, EXTENT>;

}// end namespace ph
