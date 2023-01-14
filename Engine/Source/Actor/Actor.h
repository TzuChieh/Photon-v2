#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "World/Foundation/CookedUnit.h"
#include "World/Foundation/CookOrder.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph { class CookingContext; }

namespace ph
{

class Actor : public TSdlResourceBase<ETypeCategory::Ref_Actor>
{
public:
	Actor();
	Actor(const Actor& other);

	virtual CookedUnit cook(CookingContext& ctx) = 0;
	virtual CookOrder getCookOrder() const;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Actor>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Actor");
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

}// end namespace ph
