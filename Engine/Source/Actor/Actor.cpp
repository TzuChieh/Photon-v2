#include "Actor/Actor.h"

#include <utility>

namespace ph
{

Actor::Actor() = default;

Actor::Actor(const Actor& other) = default;

Actor& Actor::operator = (const Actor& rhs)
{
	return *this;
}

void swap(Actor& first, Actor& second)
{}

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
