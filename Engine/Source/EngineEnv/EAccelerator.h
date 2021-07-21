#pragma once

namespace ph
{

enum class EAccelerator
{
	UNSPECIFIED = 0,

	BRUTE_FORCE,
	BVH,
	KDTREE,
	INDEXED_KDTREE
};

}// end namespace ph
