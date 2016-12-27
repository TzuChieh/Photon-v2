#pragma once

#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class Model;
class EmitterStorage;

class LightSampler
{
public:
	virtual ~LightSampler() = 0;

	virtual void update(const EmitterStorage& emitters) = 0;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const = 0;
};

}// end namespace ph