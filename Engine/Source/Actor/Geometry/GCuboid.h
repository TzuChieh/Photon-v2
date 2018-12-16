#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class GCuboid : public Geometry, public TCommandInterface<GCuboid>
{
public:
	GCuboid();
	explicit GCuboid(real sideLength);
	GCuboid(real sideLength, const Vector3R& offset);
	GCuboid(real xLen, real yLen, real zLen);
	GCuboid(const Vector3R& minVertex, const Vector3R& maxVertex);
	GCuboid(real xLen, real yLen, real zLen, const Vector3R& offset);
	GCuboid(const GCuboid& other);

	void genPrimitive(const PrimitiveBuildingMaterial& data,
	                  std::vector<std::unique_ptr<Primitive>>& out_primitives) const;

	GCuboid& operator = (const GCuboid& rhs);

private:
	real     m_xLen;
	real     m_yLen;
	real     m_zLen;
	Vector3R m_offset;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> cuboid            </type_name>
	<extend>    geometry.geometry </extend>

	<name> Cuboid </name>
	<description>
		A shape that is similar to cube but may contain rectangular faces. It is
		centered around origin.
	</description>

	<command type="creator">
		<input name="min-vertex" type="vector3">
			<description>Vertex in the (---) octant.</description>
		</input>
		<input name="max-vertex" type="vector3">
			<description>Vertex in the (+++) octant.</description>
		</input>
	</command>

	</SDL_interface>
*/