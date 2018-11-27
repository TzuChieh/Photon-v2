#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class Emitter;
class CookingContext;
class Geometry;
class Material;
class InputPacket;

class LightSource : public TCommandInterface<LightSource>
{
public:
	LightSource();
	virtual ~LightSource() = 0;

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

// command interface
public:
	LightSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source </category>
	<type_name> light-source </type_name>

	<name> Light Source </name>
	<description>
		The source of all energy emitting entity in the scene.
	</description>

	</SDL_interface>
*/