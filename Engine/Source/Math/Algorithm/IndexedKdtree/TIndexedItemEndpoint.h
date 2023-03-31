#pragma once

#include "Common/primitive_type.h"

namespace ph::math
{

enum class EEndpoint
{
	MIN,
	MAX
};

template<typename Index>
struct TIndexedItemEndpoint
{
	real      position;
	Index     index;
	EEndpoint type;
};

}// end namespace ph::math
