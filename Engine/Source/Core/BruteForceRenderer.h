#pragma once

#include "Core/Renderer.h"

namespace ph
{

class BruteForceRenderer final : public Renderer
{
public:
	virtual ~BruteForceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
	virtual float32 queryPercentageProgress() const override;
};

}// end namepace ph