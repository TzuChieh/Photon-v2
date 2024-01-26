#pragma once

#include "Container/detail.h"

#include <unordered_map>

namespace ph
{

/*! @brief Unordered `std::string` map with support for heterogeneous string key lookup.
Supports `std::string_view` and literal (C-style) string lookup in addition to the original
`std::string` lookup. The heterogeneous access can save redundant dynamic allocations when querying
the map.
*/
template<typename Value>
using TStdUnorderedStringMap = std::unordered_map<
	std::string, Value, detail::HeterogeneousStringHash, std::equal_to<>>;

}// end namespace ph
