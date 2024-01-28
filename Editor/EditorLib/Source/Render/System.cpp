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
		PH_LOG(System, Warning,
			"One file reading work is empty, skipping.");
	}
}

}// end anonymous namespace

System::System(ghi::GraphicsContext& graphicsCtx)
	: updateCtx()

	, m_graphicsCtx(graphicsCtx)

	, m_sceneStorage()
	, m_scenes()
	, m_removingScenes()
	, m_removedScenes()

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

	PH_ASSERT_EQ(m_sceneStorage.size(), 0);
	PH_ASSERT_EQ(m_scenes.size(), 0);
	PH_ASSERT_EQ(m_removingScenes.size(), 0);
	PH_ASSERT_EQ(m_removedScenes.size(), 0);
}

void System::addScene(std::unique_ptr<Scene> scene)
{
	if(!scene)
	{
		return;
	}

	scene->setSystem(this);
	m_scenes.push_back(scene.get());
	m_sceneStorage.add(std::move(scene));
}

void System::removeScene(Scene* scene)
{
	if(!scene)
	{
		return;
	}

	std::erase(m_scenes, scene);
	m_removingScenes.push_back(scene);
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

void System::clearRemovingScenes()
{
	m_removedScenes.insert(m_removedScenes.end(), m_removingScenes.begin(), m_removingScenes.end());
	m_removingScenes.clear();
}

void System::clearRemovedScenes()
{
	for(Scene* scene : m_removedScenes)
	{
		std::unique_ptr<Scene> removedScene = m_sceneStorage.remove(scene);
		if(!removedScene)
		{
			PH_LOG(System, Error,
				"Removed scene {} was not in the system storage. Please make sure to transfer scene "
				"ownership to the system after construction.", scene->getDebugName());
		}
	}
	m_removedScenes.clear();
}

}// end namespace ph::editor::render
