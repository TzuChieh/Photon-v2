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

class GCuboid : public Geometry, public TCommandInterface<GCuboid>
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
			<description>Vertex in the (-, -, -) octant.</description>
		</input>
		<input name="max-vertex" type="vector3">
			<description>Vertex in the (+, +, +) octant.</description>
		</input>
		<input name="px-face-uv" type="quaternion">
			<description>UV coordinates of the +x face (+y as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
		<input name="nx-face-uv" type="quaternion">
			<description>UV coordinates of the -x face (+y as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
		<input name="pz-face-uv" type="quaternion">
			<description>UV coordinates of the +z face (+y as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
		<input name="nz-face-uv" type="quaternion">
			<description>UV coordinates of the -z face (+y as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
		<input name="py-face-uv" type="quaternion">
			<description>UV coordinates of the +y face (-z as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
		<input name="ny-face-uv" type="quaternion">
			<description>UV coordinates of the -y face (+z as upward), in (min-u, min-v, max-u, max-v).</description>
		</input>
	</command>

	</SDL_interface>
*/
