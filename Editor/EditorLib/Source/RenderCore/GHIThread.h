#pragma once

#include "EditorCore/Thread/TUnbufferedFrameWorkerThread.h"
#include "RenderCore/GHIThreadUpdateContext.h"

#include <Common/primitive_type.h>
#include <Utility/Timer.h>

#include <memory>
#include <atomic>
#include <thread>

namespace ph::editor::ghi { class GraphicsContext; }

namespace ph::editor
{

class GHIThread final : public TUnbufferedFrameWorkerThread<void(ghi::GraphicsContext&)>
{
public:
	using Base = TUnbufferedFrameWorkerThread<void(ghi::GraphicsContext&)>;

	GHIThread();
	~GHIThread() override;

	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame() override;
	void onEndFrame() override;

	void addContextSwitchWork(ghi::GraphicsContext* newCtx);

	/*!
	@note Thread-safe.
	*/
	float32 getFrameTimeMs() const;

	/*!
	@note Thread-safe.
	*/
	bool isOnGHIThread() const;

private:
	void switchContext(ghi::GraphicsContext* newCtx);

	ghi::GraphicsContext* m_ctx;
	std::unique_ptr<ghi::GraphicsContext> m_nullCtx;
	GHIThreadUpdateContext m_updateCtx;
	Timer m_frameTimer;
	std::atomic<float32> m_frameTimeMs;
};

inline float32 GHIThread::getFrameTimeMs() const
{
	return m_frameTimeMs.load(std::memory_order_relaxed);
}

inline bool GHIThread::isOnGHIThread() const
{
	return std::this_thread::get_id() == getWorkerThreadId();
}

}// end namespace ph::editor
