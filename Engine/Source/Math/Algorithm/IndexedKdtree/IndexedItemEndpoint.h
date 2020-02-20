#pragma once

#include "Common/primitive_type.h"

namespace ph::math
{

enum class EEndpoint
{
	MIN,
	MAX
};

struct IndexedItemEndpoint
{
	real      position;
	int       index;// FIXME: templatize
	EEndpoint type;
};

}// end namespace ph::math
