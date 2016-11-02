#pragma once

#include "Core/Renderer.h"

namespace ph
{

class BruteForceRenderer : public Renderer
{
public:
	virtual ~BruteForceRenderer() override;

	virtual void render(const World& world, const Camera& camera, HDRFrame* const out_frame) const override;
};

}// end namepace ph