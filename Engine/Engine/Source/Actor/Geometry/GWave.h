#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/math_fwd.h"

#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

class GWave : public Geometry
{
public:
	GWave(real xLen, real yLen, real zLen);

	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	real m_xLen;
	real m_yLen;
	real m_zLen;

	static void genTessellatedRectangleXZ(const real xLen, const real zLen, const int32 numXdivs, const int32 numZdivs, std::vector<math::Vector3R>& positions);
	static bool checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen);
	static bool checkData(real xLen, real yLen, real zLen);
};

}// end namespace ph
