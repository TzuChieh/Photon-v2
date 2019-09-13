#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

private:
	DielectricInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

// command interface
public:
	explicit AbradedTranslucent(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material            </category>
	<type_name> abraded-translucent </type_name>
	<extend>    material.material   </extend>

	<name> Abraded Translucent </name>
	<description>
		Able to model translucent surfaces with variable roughnesses. Such as
		frosted glass.
	</description>

	<command type="creator">
		<input name="fresnel" type="dielectric-interface-info">
			<description>
				Fresnel model for the microsurface.
			</description>
		</input>
		<input name="roughness" type="real">
			<description>
				Isotropic surface roughness in [0, 1], the material will appear
				to be smoother with smaller roughness value.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
