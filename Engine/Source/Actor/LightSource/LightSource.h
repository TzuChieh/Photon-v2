#pragma once

#include <memory>

namespace ph
{

class CoreActor;
class InputPacket;

class LightSource
{
public:
	LightSource();
	LightSource(const InputPacket& packet);
	virtual ~LightSource() = 0;

	virtual void buildEmitter(CoreActor& coreActor) const = 0;
};

}// end namespace ph