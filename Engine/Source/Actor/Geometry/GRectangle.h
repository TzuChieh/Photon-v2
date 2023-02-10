#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class GTriangleMesh;

class GRectangle : public Geometry
{
public:
	void cook(
		CookedGeometry& out_geometry,
		const CookingContext& ctx,
		const GeometryCookConfig& config) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void setWidth(real width);
	void setHeight(real height);
	void setTexCoordScale(const real scale);

private:
	real m_width;
	real m_height;
	real m_texCoordScale;

	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);
	static bool checkData(real width, real height);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GRectangle>)
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
