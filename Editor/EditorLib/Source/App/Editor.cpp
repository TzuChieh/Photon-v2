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

Editor::~Editor()
{
	// Make sure everything is cleaned up
	PH_ASSERT_EQ(m_scenes.size(), 0);
	PH_ASSERT_EQ(m_removingScenes.size(), 0);
}

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

	cleanupRemovingScenes();
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

	renderCleanupRemovingScenes(caller);
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

void Editor::renderCleanupRemovingScenes(RenderThreadCaller& caller)
{
	for(auto& removingScene : m_removingScenes)
	{
		if(!removingScene.hasRenderCleanupDone)
		{
			PH_ASSERT(!removingScene.hasCleanupDone);
			removingScene.scene->renderCleanup(caller);
			removingScene.hasRenderCleanupDone = true;
		}
	}
}

void Editor::cleanupRemovingScenes()
{
	for(auto& removingScene : m_removingScenes)
	{
		if(removingScene.hasRenderCleanupDone && !removingScene.hasCleanupDone)
		{
			removingScene.scene->cleanup();
			removingScene.hasCleanupDone = true;
		}
	}

	std::erase_if(
		m_removingScenes,
		[](const PendingRemovalScene& removingScene)
		{
			return removingScene.hasRenderCleanupDone && removingScene.hasCleanupDone;
		});
}

void Editor::renderCleanup(RenderThreadCaller& caller)
{
	renderCleanupRemovingScenes(caller);

	// Also cleanup existing scenes
	for(auto& scene : m_scenes)
	{
		scene->renderCleanup(caller);
	}
}

void Editor::cleanup()
{
	cleanupRemovingScenes();

	// Also cleanup existing scenes
	for(auto& scene : m_scenes)
	{
		scene->cleanup();
	}
}

std::size_t Editor::createScene()
{
	DesignerScene* const scene = m_scenes.add(std::make_unique<DesignerScene>(this));
	PH_ASSERT(scene != nullptr);

	m_activeScene = scene;
	postEvent(EditContextUpdateEvent(this, EEditContextEvent::ActiveSceneChanged), onEditContextUpdate);

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

	// Reassign another scene as the active one
	m_activeScene = nullptr;
	for(std::size_t i = 0; i < m_scenes.size(); ++i)
	{
		if(i != sceneIndex)
		{
			m_activeScene = m_scenes[i];
		}
	}

	if(m_activeScene != m_scenes[sceneIndex])
	{
		postEvent(EditContextUpdateEvent(this, EEditContextEvent::ActiveSceneChanged), onEditContextUpdate);
	}

	m_removingScenes.push_back({
		.scene = m_scenes.remove(sceneIndex),
		.hasRenderCleanupDone = false,
		.hasCleanupDone = false});
}

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Dispatch queued events to listeners
	m_eventPostQueue.flushAllEvents();
}

EditContext Editor::getEditContext() const
{
	EditContext ctx;
	ctx.activeScene = m_activeScene;
	return ctx;
}

}// end namespace ph::editor
