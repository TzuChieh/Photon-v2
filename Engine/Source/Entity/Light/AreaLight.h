#pragma once

#include "Entity/Light/Light.h"

namespace ph
{

class AreaLight final : public Light
{
public:
	virtual ~AreaLight() override;

	virtual void buildEmitters(EmitterStorage* const out_data, const Entity& parentEntity) const override;

private:
	static bool checkEntityCompleteness(const Entity& entity);
};

}// end namespace ph