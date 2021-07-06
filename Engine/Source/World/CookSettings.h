#pragma once

#include "DataIO/Option.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EAccelerator
{
	UNSPECIFIED = 0,

	BRUTE_FORCE,
	BVH,
	KDTREE,
	INDEXED_KDTREE
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EAccelerator>)
{
	SdlEnumType sdlEnum("accelerator");
	sdlEnum.description("Denotes acceleration structure types.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED,    "");
	sdlEnum.addEntry(EnumType::BRUTE_FORCE,    "brute-force");
	sdlEnum.addEntry(EnumType::BVH,            "bvh");
	sdlEnum.addEntry(EnumType::KDTREE,         "kd-tree");
	sdlEnum.addEntry(EnumType::INDEXED_KDTREE, "indexed-kd-tree");

	return sdlEnum;
}

class CookSettings final : public Option
{
public:
	CookSettings();
	explicit CookSettings(EAccelerator topLevelAccelerator);

	void setTopLevelAccelerator(EAccelerator accelerator);
	EAccelerator getTopLevelAccelerator() const;

private:
	EAccelerator m_topLevelAccelerator;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<CookSettings>)
	{
		ClassType clazz("cook");
		clazz.description(
			"Settings related to the actor-cooking process.");
		clazz.baseOn<Option>();

		TSdlEnumField<OwnerType, EAccelerator> topLevelAccelerator("top-level-accelerator", &OwnerType::m_topLevelAccelerator);
		topLevelAccelerator.description("Acceleration structure used on the top level geometries.");
		topLevelAccelerator.defaultTo(EAccelerator::BVH);
		topLevelAccelerator.optional();
		clazz.addField(topLevelAccelerator);

		return clazz;
	}
};

// In-header Implementations:

inline void CookSettings::setTopLevelAccelerator(const EAccelerator accelerator)
{
	m_topLevelAccelerator = accelerator;
}

inline EAccelerator CookSettings::getTopLevelAccelerator() const
{
	return m_topLevelAccelerator;
}

}// end namespace ph
