#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <vector>

namespace ph
{

class GWave : public Geometry
{
public:
	GWave(const real xLen, const real yLen, const real zLen);
	virtual ~GWave() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	real m_xLen;
	real m_yLen;
	real m_zLen;

	static void genTessellatedRectangleXZ(const real xLen, const real zLen, const int32 numXdivs, const int32 numZdivs, std::vector<Vector3R>& positions);
	static bool checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen);
};

}// end namespace ph