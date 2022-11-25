#pragma once

namespace ph
{

enum class EScheduler
{
	Unspecified = 0,

	BULK,
	STRIPE,
	GRID,
	TILE,
	SPIRAL,
	SPIRAL_GRID
};

}// end namespace ph
