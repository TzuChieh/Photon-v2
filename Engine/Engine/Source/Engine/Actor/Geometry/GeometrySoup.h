#pragma once

#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/SDL/sdl_interface.h"

#include <vector>
#include <memory>

namespace ph
{

class GeometrySoup : public Geometry
{
public:
	GeometrySoup();

	void storeCooked(
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const override;

	void storeCookedWithBakedTransform(
		const CookingContext& ctx,
		const StaticAffineTransform& transform,
		CookedGeometry& out_geometry) const override;

	void add(const std::shared_ptr<Geometry>& geometry);

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;

public:
	PH_DEFINE_SDL_CLASS(GeometrySoup, clazz)
	{
		clazz.typeName("geometry-soup");
		clazz.docName("Geometry Soup");
		clazz.description("A collection of random geometries.");
		clazz.baseOn<Geometry>();

		TSdlReferenceArray<Geometry, GeometrySoup> geometries("geometries", &OwnerType::m_geometries);
		geometries.description("Array of references to the geometries in the soup.");
		clazz.addField(geometries);
	}
};

}// end namespace ph
