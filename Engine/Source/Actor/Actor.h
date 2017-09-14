#pragma once

#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/CookedUnit.h"
#include "Actor/CookingContext.h"

namespace ph
{

class Actor : public TCommandInterface<Actor>
{
public:
	Actor();
	Actor(const Actor& other);
	virtual ~Actor() = 0;

	virtual CookedUnit cook(CookingContext& context) const = 0;

	Actor& operator = (const Actor& rhs);

	friend void swap(Actor& first, Actor& second);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph