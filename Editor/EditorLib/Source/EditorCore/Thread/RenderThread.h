#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"

#include <memory>

namespace ph::editor
{

class RTRScene;
class RenderThreadResource;

class RenderThread : 
	public TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES, 
		void(RTRScene&)>
{
public:
	using Base = TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES,
		void(RTRScene&)>;

	RenderThread();
	~RenderThread() override;

	void onAsyncProcessWork(const Work& work) override;
	void onBeginFrame(std::size_t frameNumber, std::size_t frameCycleIndex) override;
	void onEndFrame() override;

private:
	std::unique_ptr<RenderThreadResource> m_resources;
};

}// end namespace ph::editor
