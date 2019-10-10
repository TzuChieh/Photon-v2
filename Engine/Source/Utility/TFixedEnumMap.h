#pragma once

#include "Utility/fixed_map_base.h"

namespace ph
{

template<auto KEY, typename Value>
using TFixedEnumMapEntry = TFixedMapEntry<decltype(KEY), KEY, Value>;

template<typename... Entries>
using TFixedEnumMap = TFixedMapBase<Entries...>;

}// end namespace ph
