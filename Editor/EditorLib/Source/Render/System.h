#pragma once

#include "Render/RendererScene.h"
#include "Render/RenderThreadUpdateContext.h"

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

	RenderThreadUpdateContext updateCtx;
	TUniquePtrVector<RendererScene> scenes;

	System();
	~System();

	RendererScene& getMainScene();

	void addFileReadingWork(FileReadingWork work);
	void waitAllFileReadingWorks();

	void setGraphicsContext(GraphicsContext* ctx);
	GraphicsContext& getGraphicsContext();

private:
	RendererScene* m_mainScene;
	GraphicsContext* m_graphicsCtx;

	TSPSCExecutor<FileReadingWork> m_fileReadingThread;
};

inline RendererScene& System::getMainScene()
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
