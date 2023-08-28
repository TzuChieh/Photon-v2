#pragma once

#include "EditorCore/Thread/TUnbufferedFrameWorkerThread.h"

#include <Common/primitive_type.h>
#include <Utility/Timer.h>

#include <memory>
#include <atomic>

namespace ph::editor
{

class GraphicsContext;

class GHIThread final : public TUnbufferedFrameWorkerThread<void(GraphicsContext&)>
{
public:
	using Base = TUnbufferedFrameWorkerThread<void(GraphicsContext&)>;

	GHIThread();
	~GHIThread() override;

	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame() override;
	void onEndFrame() override;

	void addSetContextWork(GraphicsContext* inCtx);

	/*!
	@note Thread-safe.
	*/
	float32 getFrameTimeMs() const;

private:
	void setContext(GraphicsContext* inCtx);

	GraphicsContext* m_ctx;
	std::unique_ptr<GraphicsContext> m_nullCtx;
	Timer m_frameTimer;
	std::atomic<float32> m_frameTimeMs;
};

inline float32 GHIThread::getFrameTimeMs() const
{
	return m_frameTimeMs.load(std::memory_order_relaxed);
}

}// end namespace ph::editor
