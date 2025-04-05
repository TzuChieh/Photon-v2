#pragma once

namespace ph
{

enum class EAttribute : unsigned int
{
	LIGHT_ENERGY = 0,
	LIGHT_ENERGY_HALF_EFFORT,
	LIGHT_ENERGY_INDEXED,
	NORMAL,
	DEPTH,
	TEXTURE_COORDINATE,
	POSITION,

	SIZE
};

}// end namespace ph
