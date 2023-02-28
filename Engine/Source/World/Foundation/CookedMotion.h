#pragma once

#include "Core/Quantity/Time.h"

namespace ph::math { class Transform; }

namespace ph
{

class MotionCookConfig final
{
public:
	Time start;
	Time end;

	MotionCookConfig();

	// TODO: e.g., prefer piecewise linear
};

class CookedMotion final
{
public:
	const math::Transform* localToWorld = nullptr;
	const math::Transform* worldToLocal = nullptr;
};

}// end namespace ph
