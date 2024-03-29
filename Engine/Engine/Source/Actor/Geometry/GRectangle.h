#pragma once

#include "Actor/Geometry/Geometry.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

namespace ph
{

class GTriangleMesh;

class GRectangle : public Geometry
{
public:
	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	GRectangle& setWidth(real width);
	GRectangle& setHeight(real height);
	GRectangle& setTexCoordScale(const real scale);

private:
	real m_width;
	real m_height;
	real m_texCoordScale;

	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);
	static bool checkData(real width, real height);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GRectangle>)
	{
		ClassType clazz("rectangle");
		clazz.docName("Rectangular Geometry");
		clazz.description("A rectangular shape on xy-plane. It is centered around origin.");
		clazz.baseOn<Geometry>();

		TSdlReal<OwnerType> width("width", &OwnerType::m_width);
		width.description("Width of the rectangle.");
		width.defaultTo(1);
		clazz.addField(width);

		TSdlReal<OwnerType> height("height", &OwnerType::m_height);
		height.description("Height of the rectangle.");
		height.defaultTo(1);
		clazz.addField(height);

		TSdlReal<OwnerType> texCoordScale("texcoord-scale", &OwnerType::m_texCoordScale);
		texCoordScale.description("A scaling factor that scales the default-generated texture coordinates.");
		texCoordScale.defaultTo(1);
		texCoordScale.optional();
		clazz.addField(texCoordScale);

		return clazz;
	}
};

}// end namespace ph
