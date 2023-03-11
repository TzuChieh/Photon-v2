#pragma once

#include <span>

namespace ph
{

template<typename T, std::size_t Extent = std::dynamic_extent>
using TSpan = std::span<T, Extent>;

template<typename T, std::size_t Extent = std::dynamic_extent>
using TSpanView = std::span<const T, Extent>;

}// end namespace ph
