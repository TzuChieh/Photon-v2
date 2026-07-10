#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TVector3.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Actor/Material/Component/sdl_component_enums.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"

#include <Common/primitive_type.h>

#include <memory>
#include <optional>

namespace ph
{

enum class EIdealSubstance
{
	Absorber = 0,
	DielectricReflector,
	DielectricTransmitter,
	MetallicReflector,
	Dielectric
};

PH_DEFINE_SDL_ENUM(EIdealSubstance, e)
{
	e.name("ideal-substance");
	e.description("Type of the physical behavior of a perfectly smooth surface.");

	e.addEntry(EnumType::Absorber,             "absorber");
	e.addEntry(EnumType::DielectricReflector,  "dielectric-reflector");
	e.addEntry(EnumType::DielectricTransmitter,"dielectric-transmitter");
	e.addEntry(EnumType::MetallicReflector,    "metallic-reflector");
	e.addEntry(EnumType::Dielectric,           "dielectric");
}

class IdealSubstance : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

	void setSubstance(EIdealSubstance substance);
	void setF0Map(std::shared_ptr<Image> f0Map);
	void setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap);
	void setTransmissionScaleMap(std::shared_ptr<Image> transmissionScaleMap);

private:
	EIdealSubstance               m_substance;
	EInterfaceFresnel             m_fresnel;
	real                          m_iorOuter;
	real                          m_iorInner;
	math::Spectrum                m_f0;
	std::shared_ptr<Image>        m_f0Map;
	math::Spectrum                m_reflectionScale;
	std::shared_ptr<Image>        m_reflectionScaleMap;
	math::Spectrum                m_transmissionScale;
	std::shared_ptr<Image>        m_transmissionScaleMap;
	std::optional<math::Spectrum> m_iorInnerN;
	std::optional<math::Spectrum> m_iorInnerK;

public:
	PH_DEFINE_SDL_CLASS(IdealSubstance, clazz)
	{
		clazz.typeName("ideal-substance");
		clazz.description(
			"Models a perfectly smooth surface with various physical properties. "
			"For paired value/map inputs, map inputs have higher precedence.");
		clazz.docName("Ideal Substance Material");
		clazz.baseOn<SurfaceMaterial>();

		TSdlEnumField<OwnerType, EIdealSubstance> substance("substance", &OwnerType::m_substance);
		substance.description("Specifying the physical property/behavior of the surface.");
		substance.required();
		substance.defaultTo(EIdealSubstance::Absorber);
		clazz.addField(substance);

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the interface.");
		fresnel.defaultTo(EInterfaceFresnel::Schlick);
		fresnel.optional();
		clazz.addField(fresnel);

		TSdlReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside the surface.");
		iorOuter.defaultTo(1);
		iorOuter.optional();
		clazz.addField(iorOuter);

		TSdlReal<OwnerType> iorInner("ior-inner", &OwnerType::m_iorInner);
		iorInner.description("The index of refraction inside the surface.");
		iorInner.defaultTo(1.5_r);
		iorInner.optional();
		clazz.addField(iorInner);

		TSdlSpectrum<OwnerType> f0("f0", math::EColorUsage::Raw, &OwnerType::m_f0);
		f0.description(
			"Surface reflectance on normal incidence. This value is expected "
			"to be given in linear-sRGB space. When this parameter is used, "
			"the underlying Fresnel model will be an approximated one (schlick) "
			"which is pretty popular in real-time graphics.");
		f0.optional();
		f0.defaultTo(math::Spectrum(1));
		clazz.addField(f0);

		TSdlReference<Image, OwnerType> f0Map("f0-map", &OwnerType::m_f0Map);
		f0Map.description(
			"Texture-mapped surface reflectance on normal incidence. "
			"This input uses the Schlick Fresnel model.");
		f0Map.optional();
		clazz.addField(f0Map);

		TSdlSpectrum<OwnerType> reflectionScale("reflection-scale", math::EColorUsage::Raw, &OwnerType::m_reflectionScale);
		reflectionScale.description(
			"A constant scaling factor for reflected energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		reflectionScale.defaultTo(math::Spectrum(1));
		reflectionScale.optional();
		clazz.addField(reflectionScale);

		TSdlReference<Image, OwnerType> reflectionScaleMap("reflection-scale-map", &OwnerType::m_reflectionScaleMap);
		reflectionScaleMap.description(
			"A texture-mapped scaling factor for reflected energy. Note that this property is only "
			"for artistic control and is not physically correct.");
		reflectionScaleMap.optional();
		clazz.addField(reflectionScaleMap);

		TSdlSpectrum<OwnerType> transmissionScale("transmission-scale", math::EColorUsage::Raw, &OwnerType::m_transmissionScale);
		transmissionScale.description(
			"A constant scaling factor for transmitted energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		transmissionScale.defaultTo(math::Spectrum(1));
		transmissionScale.optional();
		clazz.addField(transmissionScale);

		TSdlReference<Image, OwnerType> transmissionScaleMap("transmission-scale-map", &OwnerType::m_transmissionScaleMap);
		transmissionScaleMap.description(
			"A texture-mapped scaling factor for transmitted energy. Note that this property is only "
			"for artistic control and is not physically correct.");
		transmissionScaleMap.optional();
		clazz.addField(transmissionScaleMap);

		TSdlOptionalSpectrum<OwnerType> iorInnerN("ior-inner-n", math::EColorUsage::Raw, &OwnerType::m_iorInnerN);
		iorInnerN.description("The complex index of refraction (real part) inside the metallic interface.");
		clazz.addField(iorInnerN);

		TSdlOptionalSpectrum<OwnerType> iorInnerK("ior-inner-k", math::EColorUsage::Raw, &OwnerType::m_iorInnerK);
		iorInnerK.description("The complex index of refraction (imaginary part) inside the metallic interface.");
		clazz.addField(iorInnerK);
	}
};

}// end namespace ph
