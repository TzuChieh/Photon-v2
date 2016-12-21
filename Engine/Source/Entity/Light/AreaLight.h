#pragma once

#include "Entity/Light/Light.h"

namespace ph
{

class AreaLight final : public Light
{
public:
	virtual ~AreaLight() override;

	virtual void buildEmitters(std::vector<std::unique_ptr<Emitter>>* const out_emitters, const EmitterMetadata* const metadata) const override;
};

}// end namespace ph