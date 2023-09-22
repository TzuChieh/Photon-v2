#pragma once

#include "Render/Renderer/SceneRenderer.h"
#include "Render/Renderer/OfflineRenderStats.h"

#include <Utility/Concurrent/TSPSCExecutor.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Concurrent/TSynchronized.h>
#include <Utility/Concurrent/TRelaxedAtomic.h>

#include <functional>
#include <atomic>

namespace ph { class Engine; }

namespace ph::editor::render
{

class OfflineRenderConfig final
{
public:
	Path sceneFile;
	Path sceneWorkingDirectory;
	bool useCopiedScene = true;
	bool enableStatsRequest = true;
	bool enablePeekingFrame = true;
};

class OfflineRenderer : public SceneRenderer
{
public:
	OfflineRenderer();
	~OfflineRenderer() override;

	void render(const OfflineRenderConfig& config);

	/*!
	@note Thread safe.
	*/
	EOfflineRenderStage getRenderStage() const;

	/*!
	@note Thread safe.
	*/
	bool tryGetRenderStats(OfflineRenderStats* stats);

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;
	void update(const UpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;

private:
	void renderSingleStaticImageOnEngineThread(const OfflineRenderConfig& config);

	TSPSCExecutor<std::function<void(void)>> m_engineThread;
	TRelaxedAtomic<EOfflineRenderStage> m_renderStage;
	TSynchronized<OfflineRenderStats> m_syncedRenderStats;
	std::atomic_flag m_requestRenderStats;
};

inline EOfflineRenderStage OfflineRenderer::getRenderStage() const
{
	return m_renderStage.relaxedRead();
}

}// end namespace ph::editor::render
