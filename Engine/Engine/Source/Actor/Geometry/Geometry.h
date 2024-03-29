#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Core/Intersection/Primitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "SDL/sdl_interface.h"

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
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const = 0;

	// DEPRECATED
	virtual void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	virtual std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const;

	virtual std::shared_ptr<Geometry> genTriangulated() const;

	/*! @brief Create a `CookedGeometry` that contains data suitable for rendering.
	*/
	CookedGeometry* createCooked(const CookingContext& ctx) const;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Geometry>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Geometry");
		clazz.description("Defining the shape of scene elements.");
		return clazz;
	}
};

}// end namespace ph
