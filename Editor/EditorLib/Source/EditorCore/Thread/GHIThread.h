#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"

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

	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame(std::size_t frameNumber, std::size_t frameCycleIndex) override;
	void onEndFrame() override;

private:
};

}// end namespace ph::editor
