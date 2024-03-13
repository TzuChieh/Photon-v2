#pragma once

#include <span>

namespace ph
{

template<typename T, std::size_t EXTENT = std::dynamic_extent>
using TSpan = std::span<T, EXTENT>;

template<typename T, std::size_t EXTENT = std::dynamic_extent>
using TSpanView = std::span<const T, EXTENT>;

}// end namespace ph
