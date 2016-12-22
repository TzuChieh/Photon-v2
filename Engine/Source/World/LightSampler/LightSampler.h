#pragma once

#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class Entity;

class LightSampler
{
public:
	virtual ~LightSampler() = 0;

	virtual void update(const std::vector<Entity>& entities) = 0;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const = 0;
};

}// end namespace ph