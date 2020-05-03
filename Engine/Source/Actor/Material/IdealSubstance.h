#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/TVector3.h"

#include <memory>
#include <functional>

namespace ph
{

class IdealSubstance : public SurfaceMaterial, public TCommandInterface<IdealSubstance>
{
public:
	IdealSubstance();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	void asDielectricReflector(real iorInner, real iorOuter);

	// FIXME: specifying ior-outer is redundent, f0 already includes this
	void asMetallicReflector(const math::Vector3R& linearSrgbF0, real iorOuter);

	void asTransmitter(real iorInner, real iorOuter);
	void asAbsorber();

	void asDielectric(
		real iorInner, 
		real iorOuter, 
		const math::Vector3R& linearSrgbReflectionScale,
		const math::Vector3R& linearSrgbTransmissionScale);

private:
	std::function<std::unique_ptr<SurfaceOptics>(CookingContext& context)> m_opticsGenerator;

// command interface
public:
	explicit IdealSubstance(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material          </category>
	<type_name> ideal-substance   </type_name>
	<extend>    material.material </extend>

	<name> Ideal Substance </name>
	<description>
		Models a perfectly smooth surface with various physical properties.
	</description>

	<command type="creator">
		<input name="type" type="string">
			<description>
				Specifying the physical behavior of the surface. Available types are
				"dielectric-reflector", "metallic-reflector", "transmitter", "absorber",
				and "dielectric".
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
		<input name="f0-rgb" type="vector3">
			<description>
				Surface reflectance on normal incidence. This value is expected
				to be given in linear-SRGB. When this parameter is used,
				the underlying Fresnel model will be an approximated one which
				is pretty popular in real-time graphics.
			</description>
		</input>
		<input name="reflection-scale" type="vector3">
			<description>
				A scaling factor for reflected energy. Note that this is only for
				artistic control and is not physically correct. This value is expected
				to be given in linear-SRGB.
			</description>
		</input>
		<input name="transmission-scale" type="vector3">
			<description>
				A scaling factor for transmitted energy. Note that this is only for
				artistic control and is not physically correct. This value is expected
				to be given in linear-SRGB.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
