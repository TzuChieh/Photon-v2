#pragma once

#include "Common/primitive_type.h"

namespace ph
{

enum class EEndpoint
{
	MIN,
	MAX
};

struct IndexedItemEndpoint
{
	real      position;
	int       index;
	EEndpoint type;
};

}// end namespace ph