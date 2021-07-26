#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/Spectrum.h"
#include "Actor/Material/Utility/EInterfaceFresnel.h"
#include "Actor/SDLExtension/TSdlSpectrum.h"

#include <memory>
#include <optional>

namespace ph
{

class ConductorFresnel;

/*! @brief Data describing the effects when light hits an conductive interface.

Contains data and procedures for generating fresnel effects of 
conductor-dielectric interface.
*/
class ConductiveInterfaceInfo final
{
public:
	inline ConductiveInterfaceInfo() = default;

	std::unique_ptr<ConductorFresnel> genFresnelEffect() const;

private:
	EInterfaceFresnel       m_fresnel;
	Spectrum                m_f0;
	std::optional<real>     m_iorOuter;
	std::optional<Spectrum> m_iorInnerN;
	std::optional<Spectrum> m_iorInnerK;

public:
	PH_DEFINE_SDL_STRUCT(TOwnerSdlStruct<ConductiveInterfaceInfo>)
	{
		StructType ztruct("conductive-interface");
		ztruct.description("Data describing the effects when light hits an conductive interface.");

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the conductive interface.");
		fresnel.optional();
		fresnel.defaultTo(EInterfaceFresnel::SCHLICK);
		ztruct.addField(fresnel);

		TSdlSpectrum<OwnerType> f0("f0", EQuantity::RAW, &OwnerType::m_f0);
		f0.description(
			"Surface reflectance on normal incidence. This value is expected "
			"to be given in linear-SRGB space. When this parameter is used, "
			"the underlying Fresnel model will be an approximated one (schlick) "
			"which is pretty popular in real-time graphics.");
		f0.optional();
		f0.defaultTo(Spectrum(0.5_r));
		ztruct.addField(f0);

		TSdlOptionalReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside of this interface.");
		ztruct.addField(iorOuter);

		TSdlOptionalSpectrum<OwnerType> iorInnerN("ior-inner-n", EQuantity::RAW, &OwnerType::m_iorInnerN);
		iorInnerN.description("The complex index of refraction (real part) inside of this interface.");
		ztruct.addField(iorInnerN);

		TSdlOptionalSpectrum<OwnerType> iorInnerK("ior-inner-k", EQuantity::RAW, &OwnerType::m_iorInnerK);
		iorInnerK.description("The complex index of refraction (imaginary part) inside of this interface.");
		ztruct.addField(iorInnerK);

		return ztruct;
	}
};

}// end namespace ph
