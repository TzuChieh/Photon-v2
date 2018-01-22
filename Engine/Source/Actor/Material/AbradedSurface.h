#pragma once

#include "Actor/Material/Material.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class AbradedSurface : public Material, public TCommandInterface<AbradedSurface>
{
public:
	virtual ~AbradedSurface() override;

	virtual void populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* out_surfaceBehavior) const = 0;

private:


// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph