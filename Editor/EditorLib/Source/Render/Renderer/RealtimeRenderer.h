#pragma once

#include "Render/Renderer/SceneRenderer.h"

namespace ph::editor::render
{

class RealtimeRenderer : public SceneRenderer
{
public:
	RealtimeRenderer();
	~RealtimeRenderer() override;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;
	void update(const UpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;

private:
};

}// end namespace ph::editor::render
