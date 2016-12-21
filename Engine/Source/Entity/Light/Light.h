#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Emitter;
class EmitterMetadata;

class Light
{
public:
	virtual ~Light() = 0;

	virtual void buildEmitters(std::vector<std::unique_ptr<Emitter>>* const out_emitters, const EmitterMetadata* const metadata) const = 0;
};

}// end namespace ph