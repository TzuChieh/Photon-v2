#pragma once

#include "Common/primitive_type.h"

#include <memory>

namespace ph::halton_detail
{

inline constexpr std::size_t MAX_DIMENSIONS = 1024;

using TableDigit = uint16;
using PermutationTable = std::unique_ptr<TableDigit[]>;

}// end namespace ph::halton_detail
