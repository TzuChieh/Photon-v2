#pragma once

#include "Actor/PhysicalActor.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class CookingContext;

class AObserver : public PhysicalActor
{
public:
	AObserver();
	Actor(const Actor& other);

	virtual CookedUnit cook(CookingContext& context) = 0;
	virtual CookOrder getCookOrder() const;

	ETypeCategory getCategory() const override;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Actor>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.description(
			"Represents an entity in the scene. "
			"Every entity that participates in a scene is an actor.");
		return clazz;
	}
};

// In-header Implementations:

inline CookOrder Actor::getCookOrder() const
{
	return CookOrder();
}

inline ETypeCategory Actor::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
