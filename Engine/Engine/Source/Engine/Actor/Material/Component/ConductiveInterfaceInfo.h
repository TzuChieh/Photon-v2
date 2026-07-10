#pragma once

#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Material/Component/sdl_component_enums.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"
#include "Engine/Core/SurfaceBehavior/Property/ConductorFresnel.h"

#include <Common/primitive_type.h>

#include <memory>
#include <optional>

namespace ph
{

class CookingContext;

/*! @brief Data describing the effects when light hits an conductive interface.

Contains data and procedures for generating fresnel effects of 
conductor-dielectric interface.
*/
class ConductiveInterfaceInfo final
{
public:
	ConductiveInterfaceInfo();
	explicit ConductiveInterfaceInfo(const math::Spectrum& f0);

	ConductiveInterfaceInfo(
		real                  iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK);

	std::unique_ptr<ConductorFresnel> genFresnelEffect(const CookingContext& ctx) const;

	void setFresnel(EInterfaceFresnel fresnel);
	void setF0(const math::Spectrum& f0);
	void setF0Map(std::shared_ptr<Image> f0Map);
	void setIorOuter(real iorOuter);
	void setIorInnerN(const math::Spectrum& iorInnerN);
	void setIorInnerK(const math::Spectrum& iorInnerK);

private:
	EInterfaceFresnel             m_fresnel;
	math::Spectrum                m_f0;
	std::shared_ptr<Image>        m_f0Map;
	real                          m_iorOuter;
	std::optional<math::Spectrum> m_iorInnerN;
	std::optional<math::Spectrum> m_iorInnerK;

public:
	PH_DEFINE_SDL_STRUCT(ConductiveInterfaceInfo, ztruct)
	{
		ztruct.typeName("conductive-interface");
		ztruct.description("Data describing the effects when light hits an conductive interface. For paired value/map inputs, map inputs have higher precedence.");

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the conductive interface.");
		fresnel.optional();
		fresnel.defaultTo(EInterfaceFresnel::Schlick);
		ztruct.addField(fresnel);

		TSdlSpectrum<OwnerType> f0("f0", math::EColorUsage::Raw, &OwnerType::m_f0);
		f0.description(
			"Surface reflectance on normal incidence. This value is expected to be given in linear-sRGB "
			"space. When this parameter is used, the underlying Fresnel model will be an approximated "
			"one (schlick) which is pretty popular in real-time graphics. Also note that F0 already "
			"includes the information of ior-outer.");
		f0.optional();
		f0.defaultTo(math::Spectrum(1));
		ztruct.addField(f0);

		TSdlReference<Image, OwnerType> f0Map("f0-map", &OwnerType::m_f0Map);
		f0Map.description("Texture-mapped surface reflectance on normal incidence. This input uses the Schlick Fresnel model.");
		f0Map.optional();
		ztruct.addField(f0Map);

		TSdlReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside of this interface.");
		iorOuter.defaultTo(1.0_r);
		iorOuter.optional();
		ztruct.addField(iorOuter);

		TSdlOptionalSpectrum<OwnerType> iorInnerN("ior-inner-n", math::EColorUsage::Raw, &OwnerType::m_iorInnerN);
		iorInnerN.description("The complex index of refraction (real part) inside of this interface.");
		ztruct.addField(iorInnerN);

		TSdlOptionalSpectrum<OwnerType> iorInnerK("ior-inner-k", math::EColorUsage::Raw, &OwnerType::m_iorInnerK);
		iorInnerK.description("The complex index of refraction (imaginary part) inside of this interface.");
		ztruct.addField(iorInnerK);
	}
};

}// end namespace ph
