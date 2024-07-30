#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Component/DielectricInterfaceInfo.h"
#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class ThinDielectricSurface : public SurfaceMaterial
{
public:
	void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	DielectricInterfaceInfo m_interfaceInfo;
	std::shared_ptr<Image>  m_thickness;
	std::shared_ptr<Image>  m_sigmaT;
	std::shared_ptr<Image>  m_reflectionScale;
	std::shared_ptr<Image>  m_transmissionScale;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ThinDielectricSurface>)
	{
		ClassType clazz("thin-dielectric-surface");
		clazz.description(
			"Treating a single interface as a solid material with a near-negligible thickness.");
		clazz.docName("Thin Surface Material");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);

		TSdlReference<Image, OwnerType> thickness("thickness", &OwnerType::m_thickness);
		thickness.description(
			"Thickness in standard scene unit. If not provided, the surface will be treated as if "
			"there is no volumetric scattering within the material.");
		thickness.optional();
		clazz.addField(thickness);

		TSdlReference<Image, OwnerType> sigmaT("sigma-t", &OwnerType::m_sigmaT);
		sigmaT.description(
			"The volume attenuation coefficient. This parameter does not have any effect if thickness "
			"is not provided.");
		sigmaT.optional();
		clazz.addField(sigmaT);

		TSdlReference<Image, OwnerType> reflectionScale("reflection-scale", &OwnerType::m_reflectionScale);
		reflectionScale.description(
			"A scaling factor for energy reflected by the interface. Note that this property is only "
			"for artistic control and is not physically correct.");
		reflectionScale.optional();
		clazz.addField(reflectionScale);

		TSdlReference<Image, OwnerType> transmissionScale("transmission-scale", &OwnerType::m_transmissionScale);
		transmissionScale.description(
			"A scaling factor for energy transmitted by the interface. Note that this property is only "
			"for artistic control and is not physically correct.");
		transmissionScale.optional();
		clazz.addField(transmissionScale);

		return clazz;
	}
};

}// end namespace ph
