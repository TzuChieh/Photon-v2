#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "DataIO/SDL/ISdlResource.h"
#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class Emitter;
class CookingContext;
class Geometry;
class Material;

class LightSource : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_LIGHT_SOURCE;

public:
	LightSource() = default;

	// Generates the core emission part of the light source.
	//
	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const = 0;

	// A light source may need to place a corresponding geometry in the scene.
	// Override this method if there is a need for that. This method will
	// return nullptr if no geometry is needed.
	//
	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& context) const;

	// Generate a suitable material for the light source if physical entity will
	// be present in the scene. The default implementation generates a pure diffuse
	// material with moderate albedo.
	//
	virtual std::shared_ptr<Material> genMaterial(CookingContext& context) const;

	ETypeCategory getCategory() const override;
};

// In-header Implementations:

inline ETypeCategory LightSource::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
