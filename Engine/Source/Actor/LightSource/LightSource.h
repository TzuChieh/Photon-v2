#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/InputPacket.h"

#include <memory>

namespace ph
{

class Emitter;
class CookingContext;

class LightSource : public TCommandInterface<LightSource>
{
public:
	LightSource();
	virtual ~LightSource() = 0;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const = 0;

// command interface
public:
	LightSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph