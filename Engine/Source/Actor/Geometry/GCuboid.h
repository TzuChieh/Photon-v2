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
	GCuboid(const float32 xLen, const float32 yLen, const float32 zLen);
	GCuboid(const GCuboid& other);
	virtual ~GCuboid() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	GCuboid& operator = (const GCuboid& rhs);

private:
	float32 m_xLen;
	float32 m_yLen;
	float32 m_zLen;

	static bool checkData(const PrimitiveBuildingMaterial& data, const float32 xLen, const float32 yLen, const float32 zLen);
};

}// end namespace ph