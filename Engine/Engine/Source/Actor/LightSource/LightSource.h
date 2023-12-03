#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Common/primitive_type.h"
#include "SDL/sdl_interface.h"

#include <memory>
#include <string>

namespace ph { class CookingContext; }

namespace ph
{

class Emitter;
class Geometry;
class Material;

class LightSource : public TSdlResourceBase<ESdlTypeCategory::Ref_LightSource>
{
public:
	LightSource() = default;

	// Generates the core emission part of the light source.
	//
	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& ctx, EmitterBuildingMaterial&& data) const = 0;

	// A light source may need to place a corresponding geometry in the scene.
	// Override this method if there is a need for that. This method will
	// return nullptr if no geometry is needed.
	//
	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& ctx) const;

	// Generate a suitable material for the light source if physical entity will
	// be present in the scene. The default implementation generates a pure diffuse
	// material with moderate albedo.
	//
	virtual std::shared_ptr<Material> genMaterial(CookingContext& ctx) const;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<LightSource>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Light Source");
		clazz.description(
			"The source of all energy emitting entity in the scene.");
		return clazz;
	}
};

}// end namespace ph
