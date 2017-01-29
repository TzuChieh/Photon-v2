#pragma once

#include "Actor/LightSource/EmitterBuildingMaterial.h"

#include <memory>

namespace ph
{

class InputPacket;
class Emitter;

class LightSource
{
public:
	LightSource();
	LightSource(const InputPacket& packet);
	virtual ~LightSource() = 0;

	virtual std::unique_ptr<Emitter> buildEmitter(const EmitterBuildingMaterial& data) const = 0;
};

}// end namespace ph