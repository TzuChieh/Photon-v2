#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <variant>

namespace ph
{

class InputPacket;
class DielectricFresnel;

/*! @brief Data describing the effects when light hits an dielectric interface.

Contains data and procedures for generating fresnel effects of 
dielectric-dielectric interface.
*/
class DielectricInterfaceInfo
{
public:
	DielectricInterfaceInfo();
	explicit DielectricInterfaceInfo(const InputPacket& packet);

	std::unique_ptr<DielectricFresnel> genFresnelEffect() const;

private:
	bool m_useExact;
	real m_iorOuter;
	real m_iorInner;
};

}// end namespace ph

/*
	<SDL_struct>

	<type_name> dielectric-interface-info </type_name>

	<input name="fresnel-model" type="string">
		<description>
			Controls the Fresnel model used. 
			Possible values are "exact" and "schlick".
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

	</SDL_struct>
*/
