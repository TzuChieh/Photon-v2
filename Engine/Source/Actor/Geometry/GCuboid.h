#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

#include <vector>

namespace ph
{

class GCuboid final : public Geometry
{
public:
	GCuboid(const float32 xLen, const float yLen, const float zLen);
	GCuboid(const GCuboid& other);
	virtual ~GCuboid() override;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const override;

	GCuboid& operator = (const GCuboid& rhs);

private:
	float32 m_xLen;
	float32 m_yLen;
	float32 m_zLen;
};

}// end namespace ph