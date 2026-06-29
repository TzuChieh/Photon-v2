#pragma once

#include "Engine/Actor/Material/Material.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class VolumeMaterial : public Material
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override = 0;

	uint16 getOverlapPriority() const override;

private:
	uint16 m_overlapPriority;

public:
	PH_DEFINE_SDL_CLASS(VolumeMaterial, clazz)
	{
		clazz.typeName("volume-material");
		clazz.docName("Volume Material");
		clazz.baseOn<Material>();

		TSdlUInt16<OwnerType> priority("overlap-priority", &OwnerType::m_overlapPriority);
		priority.description(
			"Determines who takes precedence when multiple volumes are overlapping. "
			"Higher value has higher priority and 0 is for disabling the volume."
			"When applied to mediums, overlapping materials with equal priorities results in "
			"undefined behavior.");
		priority.defaultTo(1);
		priority.optional();
		clazz.addField(priority);
	}
};

}// end namespace ph
