#pragma once

#include "Utility/fixed_map_base.h"

namespace ph
{

namespace fixed_integer_map_detail
{
	using Key = int;
}

template<fixed_integer_map_detail::Key KEY, typename Value>
using TFixedIntegerMapEntry = TFixedMapEntry<fixed_integer_map_detail::Key, KEY, Value>;

template<typename... Entries>
using TFixedIntegerMap = TFixedMapBase<Entries...>;

}// end namespace ph
