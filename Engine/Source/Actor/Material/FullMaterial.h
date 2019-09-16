#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class InputPacket;
class CookingContext;
class SurfaceMaterial;
class VolumeMaterial;

// TODO: volume material

class FullMaterial final : public Material, public TCommandInterface<FullMaterial>
{
public:
	FullMaterial();
	FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial);

	void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_surfaceMaterial;
	std::shared_ptr<VolumeMaterial>  m_interiorMaterial;
	std::shared_ptr<VolumeMaterial>  m_exteriorMaterial;

// command interface
public:
	explicit FullMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material          </category>
	<type_name> full              </type_name>
	<extend>    material.material </extend>

	<name> Full Material </name>
	<description>
		A material model that combines surface and volume properties.
	</description>

	<command type="creator">
		<input name="surface" type="material">
			<description>A surface material.</description>
		</input>
		<input name="interior" type="material">
			<description>A volume material describing the inside of the surface.</description>
		</input>
		<input name="exterior" type="material">
			<description>A volume material describing the outside of the surface.</description>
		</input>
	</command>

	</SDL_interface>
*/
