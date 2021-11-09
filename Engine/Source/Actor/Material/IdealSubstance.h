#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Actor/Material/Utility/EInterfaceFresnel.h"
#include "Actor/SDLExtension/TSdlSpectrum.h"

#include <optional>

namespace ph
{

enum class EIdealSubstance
{
	Absorber = 0,
	DielectricReflector,
	MetallicReflector,
	Transmitter,
	Dielectric
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EIdealSubstance>)
{
	SdlEnumType sdlEnum("ideal-substance");
	sdlEnum.description("Type of the physical behavior of a perfectly smooth surface.");

	sdlEnum.addEntry(EnumType::Absorber,            "absorber");
	sdlEnum.addEntry(EnumType::DielectricReflector, "dielectric-reflector");
	sdlEnum.addEntry(EnumType::MetallicReflector,   "metallic-reflector");
	sdlEnum.addEntry(EnumType::Transmitter,         "transmitter");
	sdlEnum.addEntry(EnumType::Dielectric,          "dielectric");

	return sdlEnum;
}

class IdealSubstance : public SurfaceMaterial
{
public:
	IdealSubstance();

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;

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
	EIdealSubstance               m_substance;
	EInterfaceFresnel             m_fresnel;
	real                          m_iorOuter;
	real                          m_iorInner;
	math::Spectrum                m_f0;
	math::Spectrum                m_reflectionScale;
	math::Spectrum                m_transmissionScale;
	std::optional<math::Spectrum> m_iorInnerN;
	std::optional<math::Spectrum> m_iorInnerK;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<IdealSubstance>)
	{
		ClassType clazz("ideal-substance");
		clazz.description("Models a perfectly smooth surface with various physical properties.");
		clazz.docName("Ideal Substance Material");
		clazz.baseOn<SurfaceMaterial>();

		TSdlEnumField<OwnerType, EIdealSubstance> substance("substance", &OwnerType::m_substance);
		substance.description("Specifying the physical property/behavior of the surface.");
		substance.required();
		substance.defaultTo(EIdealSubstance::Absorber);
		clazz.addField(substance);

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the interface.");
		fresnel.optional();
		fresnel.defaultTo(EInterfaceFresnel::Schlick);
		clazz.addField(fresnel);

		TSdlReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside the surface.");
		iorOuter.optional();
		iorOuter.defaultTo(1);
		clazz.addField(iorOuter);

		TSdlReal<OwnerType> iorInner("ior-inner", &OwnerType::m_iorInner);
		iorInner.description("The index of refraction inside the surface.");
		iorInner.niceToHave();
		iorInner.defaultTo(1.5_r);
		clazz.addField(iorInner);

		TSdlSpectrum<OwnerType> f0("f0", math::EColorUsage::RAW, &OwnerType::m_f0);
		f0.description(
			"Surface reflectance on normal incidence. This value is expected "
			"to be given in linear-sRGB space. When this parameter is used, "
			"the underlying Fresnel model will be an approximated one (schlick) "
			"which is pretty popular in real-time graphics.");
		f0.optional();
		f0.defaultTo(math::Spectrum(1));
		clazz.addField(f0);

		TSdlSpectrum<OwnerType> reflectionScale("reflection-scale", math::EColorUsage::RAW, &OwnerType::m_reflectionScale);
		reflectionScale.description(
			"A scaling factor for reflected energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		reflectionScale.defaultTo(math::Spectrum(1));
		reflectionScale.optional();
		clazz.addField(reflectionScale);

		TSdlSpectrum<OwnerType> transmissionScale("transmission-scale", math::EColorUsage::RAW, &OwnerType::m_transmissionScale);
		transmissionScale.description(
			"A scaling factor for transmitted energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		transmissionScale.defaultTo(math::Spectrum(1));
		transmissionScale.optional();
		clazz.addField(transmissionScale);

		TSdlOptionalSpectrum<OwnerType> iorInnerN("ior-inner-n", math::EColorUsage::RAW, &OwnerType::m_iorInnerN);
		iorInnerN.description("The complex index of refraction (real part) inside the metallic interface.");
		clazz.addField(iorInnerN);

		TSdlOptionalSpectrum<OwnerType> iorInnerK("ior-inner-k", math::EColorUsage::RAW, &OwnerType::m_iorInnerK);
		iorInnerK.description("The complex index of refraction (imaginary part) inside the metallic interface.");
		clazz.addField(iorInnerK);

		return clazz;
	}
};

}// end namespace ph
