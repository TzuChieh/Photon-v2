#pragma once

#include "Actor/Geometry/Geometry.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>
#include <memory>

namespace ph
{

class GeometrySoup : public Geometry
{
public:
	GeometrySoup();

	void cook(
		CookedGeometry& out_geometry,
		const CookingContext& ctx,
		const GeometryCookConfig& config) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void add(const std::shared_ptr<Geometry>& geometry);

	bool addTransformed(
		const std::shared_ptr<Geometry>& geometry, 
		const math::StaticAffineTransform& transform);

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GeometrySoup>)
	{
		ClassType clazz("geometry-soup");
		clazz.docName("Geometry Soup");
		clazz.description("A collection of random geometries.");
		clazz.baseOn<Geometry>();

		TSdlReferenceArray<Geometry, GeometrySoup> geometries("geometries", &OwnerType::m_geometries);
		geometries.description("Array of references to the geometries in the soup.");
		clazz.addField(geometries);

		return clazz;
	}
};

}// end namespace ph
