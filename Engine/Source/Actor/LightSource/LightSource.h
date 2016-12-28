#pragma once

#include <memory>

namespace ph
{
class CoreActor;

class LightSource
{
public:
	virtual ~LightSource() = 0;

	virtual void buildEmitter(CoreActor& coreActor) const = 0;
};

}// end namespace ph