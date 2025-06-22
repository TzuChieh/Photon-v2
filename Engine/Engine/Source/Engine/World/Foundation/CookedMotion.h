#pragma once

#include "Engine/Core/Quantity/Time.h"

namespace ph { class Transform; }

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
	const Transform* localToWorld = nullptr;
	const Transform* worldToLocal = nullptr;
};

}// end namespace ph
