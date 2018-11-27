#pragma once

#include "Actor/Geometry/GTriangle.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <vector>
#include <memory>

namespace ph
{

class GTriangleMesh final : public Geometry, public TCommandInterface<GTriangleMesh>
{
public:
	GTriangleMesh();
	GTriangleMesh(const std::vector<Vector3R>& positions, 
	              const std::vector<Vector3R>& texCoords, 
	              const std::vector<Vector3R>& normals);
	virtual ~GTriangleMesh() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticAffineTransform& transform) const override;

	void addTriangle(const GTriangle& gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GTriangleMesh> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> triangle-mesh     </type_name>
	<extend>    geometry.geometry </extend>

	<name> Triangle Mesh </name>
	<description>
		A cluster of triangles forming a singe shape in 3-D space.
	</description>

	<command type="creator">
		<input name="positions" type="vector3-array">
			<description>
				Vertices of all triangles. Every three vector3s in the array represents
				a single triangle. The vertices are expected to be given in counterclockwise order.
			</description>
		</input>
		<input name="texture-coordinates" type="vector3-array">
			<description>
				Similar to positions, except that the array stores texture coordinates for 
				each triangle.
			</description>
		</input>
		<input name="normals" type="vector3-array">
			<description>
				Similar to positions, except that the array stores normal vectors for 
				each triangle.
			</description>
		</input>
	</command>

	</SDL_interface>
*/