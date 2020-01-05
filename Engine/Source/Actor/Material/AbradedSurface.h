#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "DataIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class AbradedSurface : public SurfaceMaterial, public TCommandInterface<AbradedSurface>
{
public:
	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override = 0;

private:


// command interface
public:
	explicit AbradedSurface(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
