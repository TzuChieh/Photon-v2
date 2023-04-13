#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <memory>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Editor, App);

Editor::Editor() = default;

Editor::~Editor() = default;

void Editor::updateScenes(const MainThreadUpdateContext& ctx)
{
	for(auto& scene : m_scenes)
	{
		scene->update(ctx);
	}
}

void Editor::renderUpdateScenes(const MainThreadRenderUpdateContext& ctx)
{
	for(auto& scene : m_scenes)
	{
		scene->renderUpdate(ctx);
	}
}

void Editor::createRenderCommandsForScenes(RenderThreadCaller& caller)
{
	for(auto& scene : m_scenes)
	{
		scene->createRenderCommands(caller);
	}
}

std::size_t Editor::createScene()
{
	DesignerScene* const scene = m_scenes.add(std::make_unique<DesignerScene>());
	PH_ASSERT(scene != nullptr);
	scene->onCreate(this);

	const std::size_t sceneIndex = m_scenes.size() - 1;
	return sceneIndex;
}

void Editor::removeScene(const std::size_t sceneIndex)
{
	if(sceneIndex >= m_scenes.size())
	{
		PH_LOG_WARNING(Editor,
			"scene not removed (scene index {} is invalid, must < {})",
			sceneIndex, m_scenes.size());

		return;
	}

	std::unique_ptr<DesignerScene> scene = m_scenes.remove(sceneIndex);
	PH_ASSERT(scene != nullptr);
	scene->onRemove();
}

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Dispatch queued events to listeners
	m_eventPostQueue.flushAllEvents();
}

}// end namespace ph::editor
