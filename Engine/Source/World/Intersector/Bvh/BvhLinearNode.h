#pragma once

#include "Common/primitive_type.h"
#include "Core/Bound/AABB3D.h"

namespace ph
{

class BvhLinearNode final
{
public:
	static BvhLinearNode makeInternal(const AABB3D& nodeAABB, std::size_t secondChildOffset, int32 splittedAxis);
	static BvhLinearNode makeLeaf(const AABB3D& nodeAABB, std::size_t primitivesOffset, int32 numPrimitives);

public:
	AABB3D aabb;

	union
	{
		std::size_t secondChildOffset;// for internal
		std::size_t primitivesOffset; // for leaf
	};

	union
	{
		int32 splittedAxis; // for internal
		int32 numPrimitives;// for leaf
	};

	BvhLinearNode();

	inline bool isLeaf() const
	{
		return m_isLeaf;
	}

	inline bool isInternal() const
	{
		return !m_isLeaf;
	}

private:
	bool m_isLeaf;
};

}// end namespace ph