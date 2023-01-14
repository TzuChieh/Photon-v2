#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class AbradedOpaque : public SurfaceMaterial
{
public:
	AbradedOpaque();

	void genSurface(CookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	ConductiveInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AbradedOpaque>)
	{
		ClassType clazz("abraded-opaque");
		clazz.docName("Abraded Opaque Material");
		clazz.description("Able to model surfaces ranging from nearly specular to extremely rough appearances.");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);

		return clazz;
	}
};

}// end namespace ph
