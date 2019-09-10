#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/Material/Utility/DielectricInterfaceInfo.h"

#include <functional>

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	//void setAlbedo(const Vector3R& albedo);

private:
	std::function<std::unique_ptr<SurfaceOptics>()> m_opticsGenerator;
	DielectricInterfaceInfo m_interfaceInfo;

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
		<input name="fresnel-type" type="string">
			<description>
				Controls the Fresnel model used. 
				Possible values are "schlick" and "exact".
			</description>
		</input>
		<input name="roughness" type="real">
			<description>
				Isotropic surface roughness in [0, 1], the material will appear
				to be smoother with smaller roughness value.
			</description>
		</input>
		<input name="ior-outer" type="real">
			<description>
				The index of refraction outside of this material.
			</description>
		</input>
		<input name="ior-inner" type="real">
			<description>
				The index of refraction inside of this material.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
