#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Utility/EInterfaceFresnel.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>
#include <variant>

namespace ph
{

class DielectricFresnel;

/*! @brief Data describing the effects when light hits an dielectric interface.

Contains data and procedures for generating fresnel effects of 
dielectric-dielectric interface.
*/
class DielectricInterfaceInfo final
{
public:
	DielectricInterfaceInfo();

	DielectricInterfaceInfo(
		EInterfaceFresnel fresnel,
		real              iorOuter,
		real              iorInner);

	std::unique_ptr<DielectricFresnel> genFresnelEffect() const;

	void setFresnel(EInterfaceFresnel fresnel);
	void setIorOuter(real iorOuter);
	void setIorInner(real iorInner);

private:
	EInterfaceFresnel m_fresnel;
	real              m_iorOuter;
	real              m_iorInner;

public:
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DielectricInterfaceInfo>)
	{
		StructType ztruct("dielectric-interface");
		ztruct.description("Data describing the effects when light hits an dielectric interface.");

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the dielectric interface.");
		fresnel.optional();
		fresnel.defaultTo(EInterfaceFresnel::Exact);
		ztruct.addField(fresnel);

		TSdlReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside of this interface.");
		iorOuter.optional();
		iorOuter.defaultTo(1);
		ztruct.addField(iorOuter);

		TSdlReal<OwnerType> iorInner("ior-inner", &OwnerType::m_iorInner);
		iorInner.description("The index of refraction inside of this interface.");
		iorInner.niceToHave();
		iorInner.defaultTo(1.5_r);
		ztruct.addField(iorInner);

		return ztruct;
	}
};

}// end namespace ph
