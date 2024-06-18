#pragma once

#include <Common/primitive_type.h>

namespace ph::lta
{

/*! @brief Type of Multiple Importance Sampling (MIS).
*/
enum class EMISStyle : uint8
{
	Balance,
	Power
};

/*! @brief Type of the set of all possible inputs for a mathematical function.
*/
enum class EDomain : uint8
{
	Empty = 0,
	SolidAngle,
	Area,
	UV01,
	UVW01,
	Discrete
};

}// end namespace ph::lta
