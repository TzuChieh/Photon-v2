#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Material/Component/ConductiveInterfaceInfo.h"
#include "Engine/Actor/Material/Component/MicrosurfaceInfo.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class AbradedOpaque : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

	void setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap);

private:
	ConductiveInterfaceInfo  m_interfaceInfo;
	MicrosurfaceInfo         m_microsurfaceInfo;
	math::Spectrum           m_reflectionScale;
	std::shared_ptr<Image>   m_reflectionScaleMap;

public:
	PH_DEFINE_SDL_CLASS(AbradedOpaque, clazz)
	{
		clazz.typeName("abraded-opaque");
		clazz.docName("Abraded Opaque Material");
		clazz.description(
			"Able to model surfaces ranging from nearly specular to extremely rough appearances. "
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
	}
};

}// end namespace ph
