#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>
#include <variant>

namespace ph
{

class InputPacket;
class ConductorFresnel;

/*! @brief Data describing the effects when light hits an conductive interface.

Contains data and procedures for generating fresnel effects of 
conductor-dielectric interface.
*/
class ConductiveInterfaceInfo final
{
public:
	ConductiveInterfaceInfo();
	explicit ConductiveInterfaceInfo(const InputPacket& packet);

	std::unique_ptr<ConductorFresnel> genFresnelEffect() const;

private:
	struct FullIor
	{
		real     outer;
		Spectrum innerN;
		Spectrum innerK;
	};

	struct SchlickIor
	{
		Spectrum f0;
	};

	bool                              m_useExact;
	std::variant<FullIor, SchlickIor> m_ior;
};

}// end namespace ph

// TODO: exact inputs
/*
	<SDL_struct>

	<type_name> conductive-interface-info </type_name>

	<input name="fresnel-model" type="string">
		<description>
			Controls the Fresnel model used. 
			Possible values are "exact" and "schlick".
		</description>
	</input>
	<input name="f0" type="vector3">
		<description>
			Surface reflectance on normal incidence. This value is expected
			to be given in linear-SRGB space. When this parameter is used,
			the underlying Fresnel model will be an approximated one (schlick)
			which is pretty popular in real-time graphics.
		</description>
	</input>

	</SDL_struct>
*/
