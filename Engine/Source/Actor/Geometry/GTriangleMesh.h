#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GTriangle.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>

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
		std::vector<math::Vector3R> positions,
		std::vector<math::Vector3R> texCoords,
		std::vector<math::Vector3R> normals);

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	std::vector<GTriangle> genTriangles() const;
	void addTriangle(const GTriangle& gTriangle);

private:
	std::vector<math::Vector3R> m_positions;
	std::vector<math::Vector3R> m_texCoords;
	std::vector<math::Vector3R> m_normals;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GTriangleMesh>)
	{
		ClassType clazz("triangle-mesh");
		clazz.docName("Triangle Mesh");
		clazz.description("A cluster of triangles forming a singe shape in 3-D space.");
		clazz.baseOn<Geometry>();

		TSdlVector3Array<OwnerType> positions("positions", &OwnerType::m_positions);
		positions.description(
			"Vertices of all triangles. Every three vector3s in the array represents a single triangle. "
			"The vertices are expected to be given in counterclockwise order.");
		positions.required();
		clazz.addField(positions);

		TSdlVector3Array<OwnerType> texCoords("texture-coordinates", &OwnerType::m_texCoords);
		texCoords.description(
			"Similar to positions, except that the array stores texture coordinates for each triangle.");
		texCoords.optional();
		clazz.addField(texCoords);

		TSdlVector3Array<OwnerType> normals("normals", &OwnerType::m_normals);
		normals.description(
			"Similar to positions, except that the array stores normal vectors for each triangle.");
		normals.optional();
		clazz.addField(normals);

		return clazz;
	}
};

}// end namespace ph
