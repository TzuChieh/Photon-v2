#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"
#include "RenderCore/RTRScene.h"

#include <memory>

namespace ph::editor
{

class GHIThread;
class GHI;

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
	void onAsyncWorkerStart() override;
	void onAsyncWorkerStop() override;
	void onBeginFrame(std::size_t frameNumber, std::size_t frameCycleIndex) override;
	void onEndFrame() override;

	void addGHIUpdateWork(GHI* updatedGHI);

private:
	RTRScene                   m_scene;
	std::unique_ptr<GHIThread> m_ghiThread;
	GHI*                       m_updatedGHI;
};

}// end namespace ph::editor
