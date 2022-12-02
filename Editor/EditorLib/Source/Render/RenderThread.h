#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"
#include "Render/RenderData.h"
#include "RenderCore/GHIThread.h"

#include <optional>

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

private:
	void beginProcessFrame();
	void endProcessFrame();

	std::optional<RenderData> m_renderData;
	GHIThread                 m_ghiThread;
	GHI*                      m_updatedGHI;
};

}// end namespace ph::editor
