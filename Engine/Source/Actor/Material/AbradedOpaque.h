#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"

namespace ph
{

class AbradedOpaque : public SurfaceMaterial, public TCommandInterface<AbradedOpaque>
{
public:
	AbradedOpaque();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

private:
	ConductiveInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

// command interface
public:
	explicit AbradedOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material          </category>
	<type_name> abraded-opaque    </type_name>
	<extend>    material.material </extend>

	<name> Abraded Opaque </name>
	<description>
		Able to model surfaces ranging from nearly specular to extremely rough
		appearances.
	</description>

	<command type="creator">
		<input name="microsurface" type="microsurface-info">
			<description>
				Describes the appearance model of surface microstructure.
			</description>
		</input>
		<input name="fresnel" type="conductive-interface-info">
			<description>
				Fresnel model for the surface microstructure.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
