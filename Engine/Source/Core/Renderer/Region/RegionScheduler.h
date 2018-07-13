#pragma once

#include "Core/Renderer/Region/Region.h"

namespace ph
{

class RegionScheduler
{
public:
	virtual ~RegionScheduler() = default;

	virtual Region getNextRegion() = 0;
};

}// end namespace ph