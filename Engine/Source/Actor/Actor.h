#pragma once

namespace ph
{

class CoreActor;

class Actor
{
public:
	virtual ~Actor() = 0;

	virtual void genCoreActor(CoreActor* const out_coreActor) const = 0;
};

}// end namespace ph