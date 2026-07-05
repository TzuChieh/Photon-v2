#pragma once

#include <cstddef>

namespace ph::math
{

class IndexedKdtreeParams final
{
public:
	std::size_t maxNodeItems = 1;
	float       traversalCost = 1.0f;
	float       interactCost = 80.0f;
	float       emptyBonus = 0.5f;
};

}// end namespace ph::math
