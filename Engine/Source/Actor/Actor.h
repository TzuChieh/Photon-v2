#pragma once

#include "DataIO/SDL/TCommandInterface.h"
#include "Actor/CookedUnit.h"
#include "Actor/CookOrder.h"

namespace ph
{

class CookingContext;
class InputPacket;

class Actor : public TCommandInterface<Actor>
{
public:
	Actor();
	Actor(const Actor& other);
	virtual ~Actor() = default;

	virtual CookedUnit cook(CookingContext& context) const = 0;
	virtual CookOrder getCookOrder() const;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);

// command interface
public:
	explicit Actor(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline CookOrder Actor::getCookOrder() const
{
	return CookOrder();
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor </category>
	<type_name> actor </type_name>

	<name> Actor </name>
	<description>
		Represents an entity in the scene. Every entity that participates in a scene
		is an actor. 
	</description>

	</SDL_interface>
*/
