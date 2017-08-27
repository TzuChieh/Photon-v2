#pragma once

#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class CookedActor;
class InputPacket;

class Actor : public TCommandInterface<Actor>
{
public:
	Actor();
	Actor(const Actor& other);
	virtual ~Actor() = 0;

	virtual void cook(CookedActor* const out_cookedActor) const = 0;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Actor>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph