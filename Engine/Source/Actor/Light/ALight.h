#pragma once

#include "Actor/PhysicalActor.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class ALight : public PhysicalActor
{
public:
	PreCookReport preCook(CookingContext& ctx) const override = 0;
	TransientVisualElement cook(CookingContext& ctx, const PreCookReport& report) override = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ALight>)
	{
		ClassType clazz("light");
		clazz.docName("Light Actor");
		clazz.description("The source of all energy emitting entity in the scene.");
		clazz.baseOn<PhysicalActor>();

		return clazz;
	}
};

}// end namespace ph
