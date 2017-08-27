#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class InputPacket;
class Emitter;

class LightSource : public TCommandInterface<LightSource>
{
public:
	LightSource();
	virtual ~LightSource() = 0;

	virtual std::unique_ptr<Emitter> buildEmitter(const EmitterBuildingMaterial& data) const = 0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<LightSource>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph