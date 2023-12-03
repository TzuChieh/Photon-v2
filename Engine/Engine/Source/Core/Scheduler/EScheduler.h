#pragma once

namespace ph
{

enum class EScheduler
{
	Unspecified = 0,

	Bulk,
	Stripe,
	Grid,
	Tile,
	Spiral,
	SpiralGrid
};

}// end namespace ph
