#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Actor/Material/Utility/EInterfaceFresnel.h"

#include <memory>
#include <functional>
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
	EIdealSubstance   m_type;
	EInterfaceFresnel m_fresnel;
	
	std::function<std::unique_ptr<SurfaceOptics>(ActorCookingContext& ctx)> m_opticsGenerator;

public:
	PH_DEFINE_SDL_STRUCT(TOwnerSdlClass<IdealSubstance>)
	{
		ClassType clazz("conductive-interface");
		ztruct.description("Data describing the effects when light hits an conductive interface.");

		TSdlEnumField<OwnerType, EInterfaceFresnel> fresnel("fresnel", &OwnerType::m_fresnel);
		fresnel.description("Type of the Fresnel for the conductive interface.");
		fresnel.optional();
		fresnel.defaultTo(EInterfaceFresnel::Schlick);
		ztruct.addField(fresnel);

		TSdlSpectrum<OwnerType> f0("f0", math::EColorUsage::RAW, &OwnerType::m_f0);
		f0.description(
			"Surface reflectance on normal incidence. This value is expected "
			"to be given in linear-sRGB space. When this parameter is used, "
			"the underlying Fresnel model will be an approximated one (schlick) "
			"which is pretty popular in real-time graphics.");
		f0.optional();
		f0.defaultTo(math::Spectrum(0.5_r));
		ztruct.addField(f0);

		TSdlOptionalReal<OwnerType> iorOuter("ior-outer", &OwnerType::m_iorOuter);
		iorOuter.description("The index of refraction outside of this interface.");
		ztruct.addField(iorOuter);

		TSdlOptionalSpectrum<OwnerType> iorInnerN("ior-inner-n", math::EColorUsage::RAW, &OwnerType::m_iorInnerN);
		iorInnerN.description("The complex index of refraction (real part) inside of this interface.");
		ztruct.addField(iorInnerN);

		TSdlOptionalSpectrum<OwnerType> iorInnerK("ior-inner-k", math::EColorUsage::RAW, &OwnerType::m_iorInnerK);
		iorInnerK.description("The complex index of refraction (imaginary part) inside of this interface.");
		ztruct.addField(iorInnerK);

		return ztruct;
	}
};

}// end namespace ph
