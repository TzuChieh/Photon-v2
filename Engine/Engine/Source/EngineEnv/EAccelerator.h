#pragma once

namespace ph
{

enum class EAccelerator
{
	Unspecified = 0,

	BruteForce,
	BVH,
	BVH4,
	BVH8,
	Kdtree,
	IndexedKdtree
};

}// end namespace ph
