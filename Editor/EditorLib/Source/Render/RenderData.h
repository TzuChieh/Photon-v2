#pragma once

#include "Render/RendererScene.h"
#include "Render/RenderThreadUpdateContext.h"

#include <Utility/TUniquePtrVector.h>
#include <Common/assertion.h>

namespace ph::editor
{

class RenderData final
{
public:
	RenderThreadUpdateContext updateCtx;
	TUniquePtrVector<RendererScene> scenes;

	RenderData();

	RendererScene& getPersistentScene();

private:
	RendererScene* m_persistentScene;
};

inline RendererScene& RenderData::getPersistentScene()
{
	PH_ASSERT(m_persistentScene != nullptr);
	return *m_persistentScene;
}

}// end namespace ph::editor
