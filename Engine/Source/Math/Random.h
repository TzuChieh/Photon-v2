#pragma once

#include "Common/primitive_type.h"

#include <atomic>

namespace ph
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

private:
	static std::atomic<int32> seed;
	static const int32 incrementation;
};

}// end namespace ph