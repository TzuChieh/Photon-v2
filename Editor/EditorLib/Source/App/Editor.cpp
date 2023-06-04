#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/Timer.h>
#include <DataIO/FileSystem/Path.h>
#include <SDL/TSdl.h>

#include <memory>
#include <utility>

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

void Editor::start()
{
	loadDefaultScene();
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

void Editor::stop()
{
	PH_LOG(Editor,
		"{} scenes to be closed",
		m_scenes.size());

	// TODO: ask whether to save current scene
}

void Editor::loadDefaultScene()
{
	createScene();
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

	m_scenes.removeAll();
}

std::size_t Editor::createScene(const std::string& name)
{
	DesignerScene* scene = nullptr;
	auto sceneIndex = static_cast<std::size_t>(-1);

	// Create new scene with required initial properties
	{
		auto newScene = std::make_unique<DesignerScene>(TSdl<DesignerScene>::make(this));

		// Optionally keep the default scene name
		if(!name.empty())
		{
			newScene->setName(name);
		}

		// Ensure no duplicated scene names
		newScene->setName(getUniqueSceneName(newScene->getName()));

		scene = m_scenes.add(std::move(newScene));
		sceneIndex = m_scenes.size() - 1;
	}

	PH_LOG(Editor,
		"created scene \"{}\"",
		scene->getName());

	setActiveScene(sceneIndex);
	return sceneIndex;
}

void Editor::openScene(const Path& sceneFilePath)
{
	// TODO
}

void Editor::setActiveScene(const std::size_t sceneIndex)
{
	DesignerScene* const sceneToBeActive = getScene(sceneIndex);
	if(sceneToBeActive != m_activeScene)
	{
		m_activeScene = sceneToBeActive;
		postEvent(EditContextUpdateEvent(this, EEditContextEvent::ActiveSceneChanged), onEditContextUpdate);
	
		if(m_activeScene)
		{
			PH_LOG(Editor,
				"scene \"{}\" is now active",
				m_activeScene->getName());
		}
		else
		{
			PH_LOG(Editor,
				"no scene is now active");
		}
	}
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
	if(m_scenes.size() == 1)
	{
		// The scene to be removed is the last one, set active one to null
		setActiveScene(static_cast<std::size_t>(-1));
	}
	else
	{
		PH_ASSERT_GE(m_scenes.size(), 2);
		for(std::size_t i = 0; i < m_scenes.size(); ++i)
		{
			if(i != sceneIndex)
			{
				setActiveScene(i);
				break;
			}
		}
	}

	m_removingScenes.push_back({
		.scene = m_scenes.remove(sceneIndex),
		.hasRenderCleanupDone = false,
		.hasCleanupDone = false});

	PH_LOG(Editor, 
		"removed scene \"{}\"", 
		m_removingScenes.back().scene->getName());
}

std::string Editor::getUniqueSceneName(const std::string& intendedName) const
{
	int suffixNumber = 1;
	while(true)
	{
		// Generating a name sequence like "name", "name (2)", "name (3)", etc.
		const std::string generatedName = 
			intendedName +
			(suffixNumber == 1 ? "" : " (" + std::to_string(suffixNumber) + ")");

		bool foundDuplicatedName = false;
		for(const auto& scene : m_scenes)
		{
			if(generatedName == scene->getName())
			{
				foundDuplicatedName = true;
				break;
			}
		}

		if(!foundDuplicatedName)
		{
			return generatedName;
		}

		++suffixNumber;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "";
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
