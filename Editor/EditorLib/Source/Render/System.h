#pragma once

#include "Render/Scene.h"
#include "Render/UpdateContext.h"

#include <Common/assertion.h>
#include <Utility/INoCopyAndMove.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/Concurrent/TSPSCExecutor.h>

#include <functional>

namespace ph::editor { class GraphicsContext; }

namespace ph::editor::render
{

class System final : private INoCopyAndMove
{
public:
	using FileReadingWork = std::function<void(void)>;

	UpdateContext updateCtx;
	TUniquePtrVector<Scene> scenes;

	System();
	~System();

	Scene& getMainScene();

	void addFileReadingWork(FileReadingWork work);
	void waitAllFileReadingWorks();

	void setGraphicsContext(GraphicsContext* ctx);
	GraphicsContext& getGraphicsContext();

private:
	Scene* m_mainScene;
	GraphicsContext* m_graphicsCtx;

	TSPSCExecutor<FileReadingWork> m_fileReadingThread;
};

inline Scene& System::getMainScene()
{
	PH_ASSERT(m_mainScene);
	return *m_mainScene;
}

inline GraphicsContext& System::getGraphicsContext()
{
	PH_ASSERT(m_graphicsCtx);
	return *m_graphicsCtx;
}

}// end namespace ph::editor::render
