#pragma once

#include "Render/RTRScene.h"
#include "Render/RenderThreadUpdateContext.h"

namespace ph::editor
{

class RenderData final
{
public:
	RenderThreadUpdateContext updateCtx;
	RTRScene                  scene;
};

}// end namespace ph::editor
