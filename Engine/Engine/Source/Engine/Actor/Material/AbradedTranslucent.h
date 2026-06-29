#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/Component/DielectricInterfaceInfo.h"
#include "Engine/Actor/Material/Component/MicrosurfaceInfo.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

private:
	DielectricInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

public:
	PH_DEFINE_SDL_CLASS(AbradedTranslucent, clazz)
	{
		clazz.typeName("abraded-translucent");
		clazz.docName("Abraded Translucent Material");
		clazz.description("Able to model translucent surfaces with variable roughnesses. Such as frosted glass.");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);
	}
};

}// end namespace ph
