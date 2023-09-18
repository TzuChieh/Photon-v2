#include "Render/System.h"
#include "editor_lib_config.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/logging.h>
#include <Common/profiling.h>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(System, Render);

namespace
{

inline void run_single_file_reading_work(const System::FileReadingWork& work)
{
	if(work)
	{
		work();
	}
	else
	{
		PH_LOG_WARNING(System,
			"One file reading work is empty, skipping.");
	}
}

}// end anonymous namespace

System::System(GraphicsContext& graphicsCtx)
	: updateCtx()

	, m_graphicsCtx(graphicsCtx)

	, m_sceneStorage()
	, m_scenes()
	, m_removedScenes()
	, m_removedSceneStorage()

	, m_fileReadingThread()
	, m_queryManager()
{
	// This is a pure render thread resident that needs to live/die on render thread
	PH_ASSERT(Threads::isOnRenderThread());

#if PH_PROFILING
	m_fileReadingThread.setOnConsumerStart(
		[]()
		{
			PH_PROFILE_NAME_THIS_THREAD("render::System File Reading thread");
		});
#endif

	m_fileReadingThread.setWorkProcessor(
		[](const FileReadingWork& work)
		{
			run_single_file_reading_work(work);
		});
	m_fileReadingThread.start();
}

System::~System()
{
	// This is a pure render thread resident that needs to live/die on render thread
	PH_ASSERT(Threads::isOnRenderThread());

	PH_ASSERT_EQ(m_scenes.size(), 0);
	PH_ASSERT_EQ(m_removedScenes.size(), 0);
	PH_ASSERT_EQ(m_sceneStorage.size(), 0);
	PH_ASSERT_EQ(m_removedSceneStorage.size(), 0);
}

void System::addScene(std::unique_ptr<Scene> scene)
{
	if(!scene)
	{
		PH_LOG_WARNING(System,
			"Adding null scene, ignoring");
		return;
	}

	scene->setSystem(this);
	m_scenes.push_back(scene.get());
	m_sceneStorage.add(std::move(scene));
}

void System::removeScene(Scene* scene)
{
	std::unique_ptr<Scene> removedScene = m_sceneStorage.remove(scene);
	if(!removedScene)
	{
		PH_LOG_WARNING(System,
			"Cannot find the scene to remove");
		return;
	}

	std::erase(m_scenes, removedScene.get());

	removedScene->removeAllContents();

	m_removedScenes.push_back(removedScene.get());
	m_removedSceneStorage.push_back({
		.scene = std::move(removedScene)});
}

void System::addFileReadingWork(FileReadingWork work)
{
	if constexpr(config::ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING)
	{
		m_fileReadingThread.addWork(std::move(work));
	}
	else
	{
		run_single_file_reading_work(work);
	}
}

void System::waitAllFileReadingWorks()
{
	if constexpr(config::ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING)
	{
		m_fileReadingThread.waitAllWorks();
	}
}

void System::processQueries()
{
	m_queryManager.processQueries(*this);
}

void System::clearRemovedScenes()
{
	m_removedScenes.clear();
	m_removedSceneStorage.clear();
}

}// end namespace ph::editor::render
