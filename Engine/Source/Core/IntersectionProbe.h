#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Intersectable;

class IntersectionProbe final
{
public:
	inline IntersectionProbe() : 
		hitT(0.0_r), hitIntersectable(nullptr)
	{

	}

public:
	real hitT;
	const Intersectable* hitIntersectable;
};

}// end namespace ph