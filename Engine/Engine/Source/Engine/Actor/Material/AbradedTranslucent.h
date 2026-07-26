#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Material/Component/DielectricInterfaceInfo.h"
#include "Engine/Actor/Material/Component/MicrosurfaceInfo.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

	void setRoughness(real roughness);
	void setRoughnessMap(std::shared_ptr<Image> roughnessMap);
	void setRoughnessVMap(std::shared_ptr<Image> roughnessVMap);
	void setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap);
	void setTransmissionScaleMap(std::shared_ptr<Image> transmissionScaleMap);

private:
	DielectricInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;
	math::Spectrum          m_reflectionScale;
	std::shared_ptr<Image>  m_reflectionScaleMap;
	math::Spectrum          m_transmissionScale;
	std::shared_ptr<Image>  m_transmissionScaleMap;

public:
	PH_DEFINE_SDL_CLASS(AbradedTranslucent, clazz)
	{
		clazz.typeName("abraded-translucent");
		clazz.docName("Abraded Translucent Material");
		clazz.description(
			"Able to model translucent surfaces with variable roughnesses, such as frosted glass. "
			"For paired value/map inputs, map inputs have higher precedence.");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);

		TSdlSpectrum<OwnerType> reflectionScale("reflection-scale", math::EColorUsage::Raw, &OwnerType::m_reflectionScale);
		reflectionScale.description(
			"A constant scaling factor for reflected energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		reflectionScale.defaultTo(math::Spectrum(1));
		reflectionScale.optional();
		clazz.addField(reflectionScale);

		TSdlReference<Image, OwnerType> reflectionScaleMap("reflection-scale-map", &OwnerType::m_reflectionScaleMap);
		reflectionScaleMap.description("See `reflection-scale`.");
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
		transmissionScaleMap.description("See `transmission-scale`.");
		transmissionScaleMap.optional();
		clazz.addField(transmissionScaleMap);
	}
};

}// end namespace ph
