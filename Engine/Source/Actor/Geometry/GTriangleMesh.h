#pragma once

#include "Actor/Geometry/GTriangle.h"

#include <vector>
#include <memory>

namespace ph
{

/*! @brief Basic triangle mesh.

This is a naive representation of triangle mesh (simply an aggregate of 
individual triangles).
*/
class GTriangleMesh : public Geometry
{
public:
	GTriangleMesh();
	GTriangleMesh(
		const std::vector<math::Vector3R>& positions,
		const std::vector<math::Vector3R>& texCoords,
		const std::vector<math::Vector3R>& normals);

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void addTriangle(const GTriangle& gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;
};

}// end namespace ph
