#include "Actor/Actor.h"

#include <utility>

namespace ph
{

Actor::Actor() = default;

Actor::Actor(const Actor& other) = default;

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

// command interface

SdlTypeInfo Actor::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "actor");
}

ExitStatus Actor::ciExecute(const std::shared_ptr<Actor>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph