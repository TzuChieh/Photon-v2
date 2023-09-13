#pragma once

#include "Render/Scene.h"
#include "Render/UpdateContext.h"
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
#include <list>

namespace ph::editor { class GraphicsContext; }

namespace ph::editor::render
{

class System final : private INoCopyAndMove
{
public:
	using FileReadingWork = std::function<void(void)>;

	UpdateContext updateCtx;

	explicit System(GraphicsContext& graphicsCtx);
	~System();

	void addScene(std::unique_ptr<Scene> scene);
	void removeScene(Scene* scene);
	TSpan<Scene*> getScenes();
	TSpan<Scene*> getRemovedScenes();

	void addFileReadingWork(FileReadingWork work);
	GraphicsContext& getGraphicsContext();
	void addQuery(Query query);

private:
	friend class SystemController;

	void waitAllFileReadingWorks();
	void processQueries();
	void clearRemovedScenes();

private:
	struct RemovedScene
	{
		std::unique_ptr<Scene> scene;
	};

	GraphicsContext& m_graphicsCtx;

	TUniquePtrVector<Scene> m_sceneStorage;
	std::vector<Scene*> m_scenes;
	std::vector<Scene*> m_removedScenes;
	std::list<RemovedScene> m_removedSceneStorage;

	TSPSCExecutor<FileReadingWork> m_fileReadingThread;
	TConcurrentQueryManager<System> m_queryManager;
};

inline TSpan<Scene*> System::getScenes()
{
	return m_scenes;
}

inline TSpan<Scene*> System::getRemovedScenes()
{
	return m_removedScenes;
}

inline GraphicsContext& System::getGraphicsContext()
{
	return m_graphicsCtx;
}

inline void System::addQuery(Query query)
{
	m_queryManager.addQuery(std::move(query));
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
