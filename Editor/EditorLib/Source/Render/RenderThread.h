#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"
#include "RenderCore/GHIThread.h"

#include <Common/primitive_type.h>
#include <Utility/Timer.h>

#include <memory>
#include <atomic>

namespace ph::editor::render { class System; }
namespace ph::editor::ghi { class GraphicsContext; }

namespace ph::editor
{

class GHIThread;

class RenderThread : 
	public TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES, 
		void(render::System&)>
{
public:
	using Base = TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES,
		void(render::System&)>;

	RenderThread();
	~RenderThread() override;

	void onAsyncProcessWork(const Work& work) override;
	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onBeginFrame() override;
	void onEndFrame() override;

	/*!
	@note Thread-safe.
	*/
	float32 getFrameTimeMs() const;

	/*!
	@note Thread-safe.
	*/
	float32 getGHIFrameTimeMs() const;

	/*!
	@note Thread-safe.
	*/
	std::thread::id getGHIWorkerThreadId() const;

	/*! @brief Set a graphics context which is persistent throughout the worker thread's lifespan.
	Must be set before the thread starts.
	*/
	void setGraphicsContext(ghi::GraphicsContext* graphicsCtx);

private:
	/*! @brief Called on render thread before the first submitted render work from main thread.
	*/
	void beforeFirstRenderWorkInFrame();

	/*! @brief Called on render thread after the last submitted render work from main thread.
	*/
	void afterLastRenderWorkInFrame();

	// Context should be set before thread start and cannot be changed. See `setGraphicsContext()`.
	ghi::GraphicsContext* m_graphicsCtx;

	// Fields that are accessed on render thread
	std::unique_ptr<render::System> m_system;
	GHIThread m_ghiThread;
	Timer m_frameTimer;

	std::atomic<float32> m_frameTimeMs;
};

inline float32 RenderThread::getFrameTimeMs() const
{
	return m_frameTimeMs.load(std::memory_order_relaxed);
}

inline float32 RenderThread::getGHIFrameTimeMs() const
{
	return m_ghiThread.getFrameTimeMs();
}

inline std::thread::id RenderThread::getGHIWorkerThreadId() const
{
	return m_ghiThread.getWorkerThreadId();
}

}// end namespace ph::editor
