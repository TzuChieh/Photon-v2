#pragma once

#include "EditorCore/Thread/TUnbufferedFrameWorkerThread.h"

#include <Common/primitive_type.h>
#include <Utility/Timer.h>

#include <memory>
#include <atomic>

namespace ph::editor
{

class GHI;

class GHIThread final : public TUnbufferedFrameWorkerThread<void(GHI&)>
{
public:
	using Base = TUnbufferedFrameWorkerThread<void(GHI&)>;

	GHIThread();
	~GHIThread() override;

	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame() override;
	void onEndFrame() override;

	void addSetGHIWork(GHI* inGHI);

	/*!
	@note Thread-safe.
	*/
	float32 getFrameTimeMs() const;

private:
	void setGHI(GHI* inGHI);

	GHI*                 m_GHI;
	std::unique_ptr<GHI> m_nullGHI;
	Timer                m_frameTimer;
	std::atomic<float32> m_frameTimeMs;
};

inline float32 GHIThread::getFrameTimeMs() const
{
	return m_frameTimeMs.load(std::memory_order_relaxed);
}

}// end namespace ph::editor
