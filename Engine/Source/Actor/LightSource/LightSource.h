#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class InputPacket;
class Emitter;
class CookingContext;

class LightSource : public TCommandInterface<LightSource>
{
public:
	LightSource();
	virtual ~LightSource() = 0;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, const EmitterBuildingMaterial& data) const = 0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph