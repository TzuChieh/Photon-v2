#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/Timer.h>

#include <memory>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Editor, App);

Editor::Editor() = default;

Editor::~Editor() = default;

void Editor::update(const MainThreadUpdateContext& ctx)
{
	// Process events
	{
		auto eventFlushTimer = Timer().start();

		flushAllEvents();

		editorStats.mainThreadEventFlushMs = eventFlushTimer.stop().getDeltaMs<float32>();
	}

	for(auto& scene : m_scenes)
	{
		scene->update(ctx);
	}
}

void Editor::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	for(auto& scene : m_scenes)
	{
		scene->renderUpdate(ctx);
	}
}

void Editor::createRenderCommands(RenderThreadCaller& caller)
{
	for(auto& scene : m_scenes)
	{
		scene->createRenderCommands(caller);
	}
}

void Editor::beforeUpdateStage()
{
	for(auto& scene : m_scenes)
	{
		scene->beforeUpdateStage();
	}
}

void Editor::afterUpdateStage()
{
	for(auto& scene : m_scenes)
	{
		scene->afterUpdateStage();
	}
}

void Editor::beforeRenderStage()
{
	for(auto& scene : m_scenes)
	{
		scene->beforeRenderStage();
	}
}

void Editor::afterRenderStage()
{
	for(auto& scene : m_scenes)
	{
		scene->afterRenderStage();
	}
}

std::size_t Editor::createScene()
{
	DesignerScene* const scene = m_scenes.add(std::make_unique<DesignerScene>(this));
	PH_ASSERT(scene != nullptr);

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

	// TODO: scene cleanup all
}

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Dispatch queued events to listeners
	m_eventPostQueue.flushAllEvents();
}

}// end namespace ph::editor
