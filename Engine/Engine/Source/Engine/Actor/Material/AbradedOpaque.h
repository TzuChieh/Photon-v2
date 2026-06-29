#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/Component/ConductiveInterfaceInfo.h"
#include "Engine/Actor/Material/Component/MicrosurfaceInfo.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class AbradedOpaque : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

private:
	ConductiveInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;

public:
	PH_DEFINE_SDL_CLASS(AbradedOpaque, clazz)
	{
		clazz.typeName("abraded-opaque");
		clazz.docName("Abraded Opaque Material");
		clazz.description("Able to model surfaces ranging from nearly specular to extremely rough appearances.");
		clazz.baseOn<SurfaceMaterial>();

		clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);
	}
};

}// end namespace ph
