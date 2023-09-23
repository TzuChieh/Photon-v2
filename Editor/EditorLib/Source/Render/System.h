#pragma once

#include "Render/Scene.h"
#include "Render/RenderThreadUpdateContext.h"
#include "EditorCore/Query/TConcurrentQueryManager.h"
#include "Render/Query/query_basics.h"

#include <Common/assertion.h>
#include <Utility/INoCopyAndMove.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/Concurrent/TSPSCExecutor.h>
#include <Utility/TSpan.h>

#include <utility>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

namespace ph::editor::ghi { class GraphicsContext; }

namespace ph::editor::render
{

class System final : private INoCopyAndMove
{
public:
	using FileReadingWork = std::function<void(void)>;

	RenderThreadUpdateContext updateCtx;

	explicit System(ghi::GraphicsContext& graphicsCtx);
	~System();

	void addScene(std::unique_ptr<Scene> scene);
	void removeScene(Scene* scene);
	TSpan<Scene*> getScenes();
	TSpan<Scene*> getRemovingScenes();
	TSpan<Scene*> getRemovedScenes();

	/*! @brief Add a file reading work, which may run on another thread.
	Behavior controlled by `config::ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING`.
	*/
	void addFileReadingWork(FileReadingWork work);

	void addQuery(Query query);
	ghi::GraphicsContext& getGraphicsContext();

private:
	friend class SystemController;

	void waitAllFileReadingWorks();
	void processQueries();
	void clearRemovingScenes();
	void clearRemovedScenes();

private:
	ghi::GraphicsContext& m_graphicsCtx;

	TUniquePtrVector<Scene> m_sceneStorage;
	std::vector<Scene*> m_scenes;
	std::vector<Scene*> m_removingScenes;
	std::vector<Scene*> m_removedScenes;

	TSPSCExecutor<FileReadingWork> m_fileReadingThread;
	TConcurrentQueryManager<System> m_queryManager;
};

inline TSpan<Scene*> System::getScenes()
{
	return m_scenes;
}

inline TSpan<Scene*> System::getRemovingScenes()
{
	return m_removingScenes;
}

inline TSpan<Scene*> System::getRemovedScenes()
{
	return m_removedScenes;
}

inline void System::addQuery(Query query)
{
	m_queryManager.addQuery(std::move(query));
}

inline ghi::GraphicsContext& System::getGraphicsContext()
{
	return m_graphicsCtx;
}

class SystemController final : private INoCopyAndMove
{
public:
	explicit SystemController(System& sys)
		: m_sys(sys)
	{}

	void waitAllFileReadingWorks()
	{
		m_sys.waitAllFileReadingWorks();
	}

	void processQueries()
	{
		m_sys.processQueries();
	}

	void clearRemovingScenes()
	{
		m_sys.clearRemovingScenes();
	}

	void clearRemovedScenes()
	{
		m_sys.clearRemovedScenes();
	}

	System* operator -> ()
	{
		return &m_sys;
	}

private:
	System& m_sys;
};

}// end namespace ph::editor::render
