#include "Actor/Actor.h"

#include <utility>

namespace ph
{

Actor::Actor() = default;

Actor::Actor(const Actor& other) = default;

Actor::Actor(const InputPacket& packet) : 
	Actor()
{

}

Actor::~Actor() = default;

void swap(Actor& first, Actor& second)
{
	// nothing to swap
}

Actor& Actor::operator = (const Actor& rhs)
{
	// nothing to assign

	return *this;
}

}// end namespace ph