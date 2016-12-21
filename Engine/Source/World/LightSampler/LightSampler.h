#pragma once

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
};

}// end namespace ph