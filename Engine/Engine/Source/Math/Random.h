#pragma once

#include <Common/primitive_type.h>

namespace ph::math
{

class Random final
{
	// Method Notations
	//
	// i: including
	// e: excluding
	// L: lower bound
	// U: upper bound
	//
	// Examples
	//
	// ...i0_e1  means [0, 1)
	// ...e9_e12 means (9, 12)

public:
	static real        genUniformReal_i0_e1();
	static std::size_t genUniformIndex_iL_eU(std::size_t lowerBound, 
	                                         std::size_t upperBound);
};

}// end namespace ph::math
