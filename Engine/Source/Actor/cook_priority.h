#pragma once

#include <limits>

namespace ph
{

using CookPriority = int;

enum class ECookPriority : CookPriority
{
	HIGH = 1,
	NORMAL = std::numeric_limits<CookPriority>::max() / 2,
	LOW
};

}// end namespace ph
