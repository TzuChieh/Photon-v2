#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial
{
public:
	AbradedTranslucent();

	void genSurface(CookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	DielectricInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AbradedTranslucent>)
	{
		ClassType clazz("abraded-translucent");
		clazz.docName("Abraded Translucent Material");
		clazz.description("Able to model translucent surfaces with variable roughnesses. Such as frosted glass.");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);

		return clazz;
	}
};

}// end namespace ph
