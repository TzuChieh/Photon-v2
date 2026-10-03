#pragma once

#include "Engine/Actor/PhysicalActor.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class ALight : public PhysicalActor
{
public:
	/*! Guaranteed to provide primitives view if the emitters generated have intersectable geometry
	(i.e., emitting light from primitives that rays can hit). Emitters and primitives are either in
	one-to-one mapping, or in one-to-many mapping (all primitives correspond to one emitter).
	Lights without intersectable geometry keep their sampling primitives outside this view.
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
