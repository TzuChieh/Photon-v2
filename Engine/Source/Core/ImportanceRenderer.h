#pragma once

#include "Core/Renderer.h"

namespace ph
{

class ImportanceRenderer : public Renderer
{
public:
	virtual ~ImportanceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
};

}// end namespace ph