#include "Render/Renderer/RealtimeRenderer.h"

namespace ph::editor::render
{

RealtimeRenderer::RealtimeRenderer()

	: SceneRenderer()
{}

RealtimeRenderer::~RealtimeRenderer()
{}

void RealtimeRenderer::setupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void RealtimeRenderer::cleanupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void RealtimeRenderer::update(const RenderThreadUpdateContext& ctx)
{
	// TODO
}

void RealtimeRenderer::createGHICommands(GHIThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor::render
