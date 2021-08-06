#pragma once

namespace ph
{

enum class ESampleFilter
{
	UNSPECIFIED = 0,

	BOX,
	GAUSSIAN,
	MITCHELL_NETRAVALI,
	BLACKMAN_HARRIS
};

}// end namespace ph