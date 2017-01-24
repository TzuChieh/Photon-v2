#pragma once

namespace ph
{

class CoreActor;
class InputPacket;

class Actor
{
public:
	Actor();
	Actor(const InputPacket& packet);
	virtual ~Actor() = 0;

	virtual void genCoreActor(CoreActor* const out_coreActor) const = 0;
};

}// end namespace ph