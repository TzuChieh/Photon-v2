#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <vector>

namespace ph
{

class GTriangleMesh;

class GSphere : public Geometry, public TCommandInterface<GSphere>
{
public:
	GSphere();
	GSphere(real radius);
	GSphere(const GSphere& other);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const override;

	GSphere& operator = (const GSphere& rhs);

private:
	real m_radius;

	std::size_t addVertex(const Vector3R& vertex, std::vector<Vector3R>* const out_vertices) const;
	std::size_t addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<Vector3R>* const out_vertices) const;
	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GSphere> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> sphere            </type_name>
	<extend>    geometry.geometry </extend>

	<name> Sphere </name>
	<description>
		A perfectly round shape centering around origin. 
	</description>

	<command type="creator">
		<input name="radius" type="real">
			<description>Size of the sphere.</description>
		</input>
	</command>

	</SDL_interface>
*/