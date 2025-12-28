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
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const override;

	void add(const std::shared_ptr<Geometry>& geometry);

	bool addTransformed(
		const std::shared_ptr<Geometry>& geometry, 
		const StaticAffineTransform& transform);

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
