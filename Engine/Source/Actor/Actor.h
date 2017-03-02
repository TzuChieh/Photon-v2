#pragma once

namespace ph
{

class CookedActor;
class InputPacket;

class Actor
{
public:
	Actor();
	Actor(const Actor& other);
	Actor(const InputPacket& packet);
	virtual ~Actor() = 0;

	virtual void cook(CookedActor* const out_cookedActor) const = 0;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);
};

}// end namespace ph