#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

#include <vector>

namespace ph
{

class GCuboid final : public Geometry
{
public:
	GCuboid(const real xLen, const real yLen, const real zLen);
	GCuboid(const GCuboid& other);
	virtual ~GCuboid() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	GCuboid& operator = (const GCuboid& rhs);

private:
	real m_xLen;
	real m_yLen;
	real m_zLen;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen);
};

}// end namespace ph