#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB2D.h"

#include <array>
#include <vector>
#include <memory>

namespace ph
{

class GCuboid : public Geometry
{
public:
	GCuboid();
	explicit GCuboid(real sideLength);
	GCuboid(real sideLength, const math::Vector3R& offset);
	GCuboid(real xLen, real yLen, real zLen);
	GCuboid(const math::Vector3R& minVertex, const math::Vector3R& maxVertex);
	GCuboid(real xLen, real yLen, real zLen, const math::Vector3R& offset);
	GCuboid(const GCuboid& other);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTriangulated() const override;

	GCuboid& operator = (const GCuboid& rhs);

private:
	math::Vector3R              m_size;
	math::Vector3R              m_offset;
	std::array<math::AABB2D, 6> m_faceUVs;

	static bool checkData(
		const PrimitiveBuildingMaterial& data, 
		const real xLen, const real yLen, const real zLen);
	static std::array<math::AABB2D, 6> genNormalizedFaceUVs();
};

}// end namespace ph
