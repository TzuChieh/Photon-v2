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

Actor& Actor::operator = (const Actor& rhs)
{
	// nothing to assign

	return *this;
}

void swap(Actor& first, Actor& second)
{
	// nothing to swap
}

}// end namespace ph