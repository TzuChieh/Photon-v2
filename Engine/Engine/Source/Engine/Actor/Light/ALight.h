#pragma once

#include "Engine/Actor/PhysicalActor.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class ALight : public PhysicalActor
{
public:
	/*! Guaranteed to provide primitives view if the emitters generated are based on primitives 
	(i.e., emitting light from primitives). Emitters and primitives are either in one-to-one mapping, 
	or in one-to-many mapping (all primitives correspond to one emitter).
	*/
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override = 0;

public:
	PH_DEFINE_SDL_CLASS(ALight, clazz)
	{
		clazz.typeName("light");
		clazz.docName("Light Actor");
		clazz.description("The source of all energy emitting entity in the scene.");
		clazz.baseOn<PhysicalActor>();
	}
};

}// end namespace ph
