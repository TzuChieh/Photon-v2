#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Emitter;
class Entity;
class EmitterStorage;

class Light
{
public:
	virtual ~Light() = 0;

	virtual void buildEmitters(EmitterStorage* const out_data, const Entity& parentEntity) const = 0;
};

}// end namespace ph