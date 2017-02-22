#pragma once

#include "Common/primitive_type.h"

#include <random>
#include <atomic>

namespace ph
{

class Random final
{
	// Method Notations
	//
	// i: include
	// e: exclude
	//
	// Examples
	//
	// ...i0_e1  means [0, 1)
	// ...e9_e12 means (9, 12)

public:
	static real genUniformReal_i0_e1();

private:
	static const std::uniform_real_distribution<real> distribution;
	static std::atomic<int32> seed;
	static const int32 incrementation;
};

}// end namespace ph