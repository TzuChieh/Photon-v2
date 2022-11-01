#pragma once

#include "RenderCore/RTRScene.h"
#include "RenderCore/RenderThreadUpdateContext.h"

namespace ph::editor
{

class RenderData final
{
public:
	RenderThreadUpdateContext updateCtx;
	RTRScene                  scene;
};

}// end namespace ph::editor
