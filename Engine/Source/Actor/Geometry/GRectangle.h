#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class GTriangleMesh;

class GRectangle : public Geometry, public TCommandInterface<GRectangle>
{
public:
	GRectangle();
	GRectangle(real width, real height);
	virtual ~GRectangle() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticAffineTransform& transform) const override;

	void setTexCoordScale(const real scale);

private:
	real m_width;
	real m_height;
	real m_texCoordScale;

	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GRectangle> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> rectangle         </type_name>
	<extend>    geometry.geometry </extend>

	<name> Rectangle </name>
	<description>
		A rectangular shape on xy-plane. It is centered around origin.
	</description>

	<command type="creator">
		<input name="width" type="real">
			<description>Width of the rectangle.</description>
		</input>
		<input name="height" type="real">
			<description>Height of the rectangle.</description>
		</input>
		<input name="texcoord-scale" type="real">
			<description>
				A scaling factor that scales the default-generated texture 
				coordinates.
			</description>
		</input>
	</command>

	</SDL_interface>
*/