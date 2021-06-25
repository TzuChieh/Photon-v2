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

}// end namespace ph
