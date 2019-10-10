#include "Actor/Actor.h"

#include <utility>

namespace ph
{

Actor::Actor() = default;

Actor::Actor(const Actor& other) = default;

Actor& Actor::operator = (const Actor& rhs)
{
	// nothing to assign

	return *this;
}

void swap(Actor& first, Actor& second)
{
	// nothing to swap
}

// command interface

Actor::Actor(const InputPacket& packet) : 
	Actor()
{}

SdlTypeInfo Actor::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "actor");
}

void Actor::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
