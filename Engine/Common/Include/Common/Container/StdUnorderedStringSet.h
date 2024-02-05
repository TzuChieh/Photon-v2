#pragma once

#include "Common/Container/detail.h"

#include <unordered_set>

namespace ph
{

/*! @brief Unordered `std::string` set with support for heterogeneous string key lookup.
Supports `std::string_view` and literal (C-style) string lookup in addition to the original
`std::string` lookup. The heterogeneous access can save redundant dynamic allocations when querying
the map.
*/
using StdUnorderedStringSet = std::unordered_set<
	std::string, detail::HeterogeneousStringHash, std::equal_to<>>;

}// end namespace ph
