#pragma once

#include "Render/IDynamicSceneResource.h"

#include <Utility/INoCopyAndMove.h>

namespace ph::editor::render
{

class SceneRenderer : public IDynamicSceneResource
{
public:
	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
	void update(const UpdateContext& ctx) override = 0;
	void createGHICommands(GHIThreadCaller& caller) override = 0;

	EProcessOrder getProcessOrder() const override;
	bool isDynamic() const override;
};

inline EProcessOrder SceneRenderer::getProcessOrder() const
{
	return EProcessOrder::ViewRender;
}

inline bool SceneRenderer::isDynamic() const
{
	return true;
}

}// end namespace ph::editor::render
