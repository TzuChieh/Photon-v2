#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class CookedGeometry;
class GeometryCookConfig;
class CookingContext;
class PrimitiveBuildingMaterial;

class Geometry : public TSdlResourceBase<ETypeCategory::Ref_Geometry>
{
public:
	/*! @brief Store data suitable for rendering into `out_geometry`.
	*/
	virtual void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx,
		const GeometryCookConfig& config) const = 0;

	// DEPRECATED
	virtual void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	virtual std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const;

	virtual std::shared_ptr<Geometry> genTriangulated() const;

	/*! @brief Create a `CookedGeometry` that contains data suitable for rendering.
	*/
	CookedGeometry* createCooked(
		const CookingContext& ctx,
		const GeometryCookConfig& config) const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Geometry>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Geometry");
		clazz.description("Defining the shape of scene elements.");
		return clazz;
	}
};

}// end namespace ph
