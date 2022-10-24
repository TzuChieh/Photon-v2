#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"

#include <memory>

namespace ph::editor
{

class GHI;

class GHIThread : 
	public TFrameWorkerThread<
		config::NUM_GHI_THREAD_BUFFERED_FRAMES,
		void(GHI&)>
{
public:
	using Base = TFrameWorkerThread<
		config::NUM_GHI_THREAD_BUFFERED_FRAMES,
		void(GHI&)>;

	GHIThread();
	~GHIThread() override;

	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame(std::size_t frameNumber, std::size_t frameCycleIndex) override;
	void onEndFrame() override;

	void addSetGHIWork(GHI* ghi);

private:
	void setGHI(GHI* ghi);

	GHI*                 m_ghi;
	std::unique_ptr<GHI> m_defaultGHI;
};

}// end namespace ph::editor
