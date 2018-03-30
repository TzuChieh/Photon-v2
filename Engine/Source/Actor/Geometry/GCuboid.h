#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <vector>

namespace ph
{

class GCuboid final : public Geometry
{
public:
	GCuboid();
	GCuboid(real sideLength);
	GCuboid(real sideLength, const Vector3R& offset);
	GCuboid(real xLen, real yLen, real zLen);
	GCuboid(const Vector3R& minVertex, const Vector3R& maxVertex);
	GCuboid(real xLen, real yLen, real zLen, const Vector3R& offset);
	GCuboid(const GCuboid& other);
	virtual ~GCuboid() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	GCuboid& operator = (const GCuboid& rhs);

private:
	real     m_xLen;
	real     m_yLen;
	real     m_zLen;
	Vector3R m_offset;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen);
};

}// end namespace ph