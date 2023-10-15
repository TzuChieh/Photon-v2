#pragma once

#include "Render/Renderer/SceneRenderer.h"
#include "Render/Renderer/OfflineRenderStats.h"
#include "Render/Renderer/OfflineRenderPeek.h"

#include <Common/primitive_type.h>
#include <Utility/Concurrent/TSPSCExecutor.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Concurrent/TSynchronized.h>
#include <Utility/Concurrent/TRelaxedAtomic.h>
#include <Frame/TFrame.h>
#include <Math/Geometry/TAABB2D.h>

#include <functional>
#include <atomic>
#include <thread>

namespace ph { class Engine; }
namespace ph { class Renderer; }
namespace ph::editor { class RenderConfig; }

namespace ph::editor::render
{

class OfflineRenderer : public SceneRenderer
{
public:
	OfflineRenderer();
	~OfflineRenderer() override;

	void render(const RenderConfig& config);

	/*!
	@note Thread safe.
	*/
	EOfflineRenderStage getRenderStage() const;

	/*! @brief Make an attempt to retrieve render statistics from the render engine.
	@return `true` if the attempt was successful and results are stored in `stats`. `false` otherwise.
	@note Thread safe.
	*/
	bool tryGetRenderStats(OfflineRenderStats* stats);

	bool tryGetRenderPeek(OfflineRenderPeek* peek, bool shouldUpdateInput = false);

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;
	void update(const RenderThreadUpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;

private:
	using EngineWork = std::function<void(void)>;

	struct FrameData
	{
		HdrRgbFrame frame;
		math::TAABB2D<int32> updatedRegion = math::TAABB2D<int32>::makeEmpty();
	};

	void renderSingleStaticImageOnEngineThread(const RenderConfig& config);

	/*!
	@note Thread safe.
	*/
	void setRenderStage(EOfflineRenderStage stage);

	std::jthread makeStatsRequestThread(Renderer* renderer, uint32 minPeriodMs);
	std::jthread makePeekFrameThread(Renderer* renderer, uint32 minPeriodMs);

	TSPSCExecutor<EngineWork> m_engineThread;
	TRelaxedAtomic<EOfflineRenderStage> m_renderStage;
	TSynchronized<OfflineRenderStats> m_syncedRenderStats;
	TSynchronized<OfflineRenderPeek> m_syncedRenderPeek;
	OfflineRenderPeek::Input m_cachedRenderPeekInput;
	TSynchronized<FrameData> m_synchedFrameData;
	std::atomic_flag m_requestRenderStats;
	std::atomic_flag m_requestRenderPeek;
};

inline EOfflineRenderStage OfflineRenderer::getRenderStage() const
{
	return m_renderStage.relaxedRead();
}

inline void OfflineRenderer::setRenderStage(EOfflineRenderStage stage)
{
	m_renderStage.relaxedWrite(stage);
}

}// end namespace ph::editor::render
