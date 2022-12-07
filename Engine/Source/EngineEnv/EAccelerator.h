#pragma once

namespace ph
{

enum class EAccelerator
{
	Unspecified = 0,

	BruteForce,
	BVH,
	Kdtree,
	IndexedKdtree
};

}// end namespace ph
