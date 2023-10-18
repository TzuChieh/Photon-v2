#include "Designer/Render/OfflineRenderAgent.h"
#include "Designer/DesignerScene.h"
#include "Designer/Render/RenderConfig.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Renderer/OfflineRenderer.h"
#include "Render/Scene.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/RenderThread.h"

#include <SDL/SceneDescription.h>
#include <Common/logging.h>

#include <utility>
#include <memory>

namespace ph::editor
{

RenderConfig OfflineRenderAgent::getRenderConfig() const
{
	const ResourceIdentifier& descLink = getScene().getRenderDescriptionLink();
	const SceneDescription& desc = getScene().getRenderDescription();

	return {
		.sceneFile = descLink.getPath(),
		.sceneWorkingDirectory = desc.getWorkingDirectory(),
		.outputDirectory = desc.getWorkingDirectory(),
		.outputName = getName(),
		.outputFileFormat = m_outputFileFormat,
		.useCopiedScene = m_useCopiedScene,
		.enableStatsRequest = m_enableStatsRequest,
		.enablePeekingFrame = m_enablePeekingFrame};
}

void OfflineRenderAgent::render(RenderConfig config)
{
	if(!m_renderer)
	{
		PH_DEFAULT_LOG_ERROR(
			"Cannot render. No renderer present.");
		return;
	}

	// Save the scene before rendering for most up-do-date scene description
	Editor& editor = getScene().getEditor();
	editor.saveScene(editor.getSceneIndex(&getScene()));

	Threads::getRenderThread().addWork(
		[renderer = m_renderer, config](render::System& /* sys */)
		{
			renderer->render(config);
		});
}

void OfflineRenderAgent::renderInit(RenderThreadCaller& caller)
{
	Base::renderInit(caller);

	auto renderer = std::make_unique<render::OfflineRenderer>();
	m_renderer = renderer.get();
	caller.add(
		[&scene = getScene().getRendererScene(), renderer = std::move(renderer)](render::System& /* sys */) mutable
		{
			scene.addDynamicResource(std::move(renderer));
		});

	getScene().addRendererBinding({
		.agent = this,
		.offlineRenderer = m_renderer});
}

void OfflineRenderAgent::renderUninit(RenderThreadCaller& caller)
{
	getScene().removeRendererBinding(this);

	caller.add(
		[&scene = getScene().getRendererScene(), renderer = m_renderer](render::System& /* sys */)
		{
			scene.removeResource(renderer);
		});
	m_renderer = nullptr;

	Base::renderUninit(caller);
}

void OfflineRenderAgent::update(const MainThreadUpdateContext& ctx)
{
	Base::update(ctx);
}

void OfflineRenderAgent::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	Base::renderUpdate(ctx);
}

}// end namespace ph::editor
