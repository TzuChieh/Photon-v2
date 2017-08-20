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

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const = 0;

private:


// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<AbradedSurface>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph