#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/SDL/ISdlResource.h"

namespace ph
{

class SurfaceBehavior;
class BSDFcos;
class InputPacket;

class Material : public TCommandInterface<Material>, public ISdlResource
{
public:
	Material();
	virtual ~Material() = 0;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const = 0;

// command interface
public:
	Material(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Material>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph