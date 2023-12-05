#pragma once

#include "Math/TVector2.h"

#include <Common/assertion.h>

#include <cstddef>

namespace ph
{

class SGUtil final
{
public:
	static void genArray2dStrata(math::Vector2R* out_coordArray2Ds, std::size_t numXs, std::size_t numYs);
	static void genArray2dLatinHypercube(math::Vector2R* out_coordArray2Ds, std::size_t num2Ds);

	template<typename T>
	static void shuffleDurstenfeld(T* out_array, std::size_t arraySize);
};

// method template implementations:

template<typename T>
void SGUtil::shuffleDurstenfeld(T* const out_array, const std::size_t arraySize)
{
	for(std::size_t i = arraySize - 1; i > 0; i--)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph
