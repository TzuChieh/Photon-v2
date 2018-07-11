#pragma once

namespace ph
{

// TODO: attribute should move out of folder <Estimator>

enum class EAttribute : unsigned int
{
	LIGHT_ENERGY = 0,
	POSITION,
	NORMAL,
	TEXTURE_COORDINATE,
	DEPTH,
	LIGHT_ENERGY_INDEXED,

	SIZE = LIGHT_ENERGY_INDEXED + 1
};

}// end namespace ph