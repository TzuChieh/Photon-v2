#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/SDL/sdl_interface.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class Primitive;
class CookedGeometry;
class CookingContext;
class PrimitiveBuildingMaterial;

class Geometry : public TSdlResourceBase<ESdlTypeCategory::Ref_Geometry>
{
public:
	/*! @brief Store data suitable for rendering into `out_geometry`.
	*/
	virtual void storeCooked(
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const = 0;

	virtual std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const;

	virtual std::shared_ptr<Geometry> genTriangulated() const;

	/*! @brief Cook geometry using the provided context and return the cooked data.
	*/
	void cook(const CookingContext& ctx, CookedGeometry& out_geometry) const;

public:
	PH_DEFINE_SDL_CLASS(Geometry, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Geometry");
		clazz.description("Defining the shape of scene elements.");
	}
};

}// end namespace ph
