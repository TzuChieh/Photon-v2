#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"

#include <functional>
#include <memory>

namespace ph
{

class AbradedOpaque : public SurfaceMaterial, public TCommandInterface<AbradedOpaque>
{
public:
	AbradedOpaque();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

private:
	std::function<std::unique_ptr<SurfaceOptics>()> m_opticsGenerator;
	ConductiveInterfaceInfo m_interfaceInfo;

// command interface
public:
	explicit AbradedOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::function<std::unique_ptr<SurfaceOptics>()> loadITR(const InputPacket& packet);
	static std::function<std::unique_ptr<SurfaceOptics>()> loadATR(const InputPacket& packet);
	//static std::unique_ptr<FresnelEffect> loadFresnelEffect(const InputPacket& packet);
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
		<input name="type" type="string">
			<description>
				Possible value are "iso-metallic-ggx" and "aniso-metallic-ggx",
				for isotropic and anisotropic surface appearances, respectively.
			</description>
		</input>
		<input name="roughness" type="real">
			<description>
				Isotropic surface roughness in [0, 1], the material will appear
				to be smoother with smaller roughness value.
			</description>
		</input>
		<input name="f0" type="vector3">
			<description>
				Surface reflectance on normal incidence. This value is expected
				to be given in linear-SRGB space. When this parameter is used,
				the underlying Fresnel model will be an approximated one which
				is pretty popular in real-time graphics.
			</description>
		</input>
		<input name="roughness-u" type="real">
			<description>
				Similar to the roughness parameter, but is used for anisotropic 
				surface appearances. This value controls the U component of 
				surface roughness.
			</description>
		</input>
		<input name="roughness-v" type="real">
			<description>
				Similar to the roughness parameter, but is used for anisotropic
				surface appearances. This value controls the V component of
				surface roughness.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
