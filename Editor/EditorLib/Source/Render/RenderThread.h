#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"
#include "Render/RenderData.h"
#include "RenderCore/GHIThread.h"

#include <Common/primitive_type.h>
#include <Utility/Timer.h>

#include <optional>
#include <atomic>

namespace ph::editor
{

class GHIThread;
class GHI;

class RenderThread : 
	public TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES, 
		void(RenderData&)>
{
public:
	using Base = TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES,
		void(RenderData&)>;

	RenderThread();
	~RenderThread() override;

	void onAsyncProcessWork(const Work& work) override;
	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onBeginFrame() override;
	void onEndFrame() override;

	void addGHIUpdateWork(GHI* updatedGHI);

	/*!
	@note Thread-safe.
	*/
	float32 getFrameTimeMs() const;

	/*!
	@note Thread-safe.
	*/
	float32 getGHIFrameTimeMs() const;

private:
	void beginProcessFrame();
	void endProcessFrame();

	std::optional<RenderData> m_renderData;
	GHIThread                 m_ghiThread;
	GHI*                      m_updatedGHI;
	Timer                     m_frameTimer;
	std::atomic<float32>      m_frameTimeMs;
};

inline float32 RenderThread::getFrameTimeMs() const
{
	return m_frameTimeMs.load(std::memory_order_relaxed);
}

inline float32 RenderThread::getGHIFrameTimeMs() const
{
	return m_ghiThread.getFrameTimeMs();
}

}// end namespace ph::editor
