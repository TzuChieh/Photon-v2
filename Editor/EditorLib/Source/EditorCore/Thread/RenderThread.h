#pragma once

#include "EditorCore/Thread/TFrameWorkerThread.h"
#include "editor_lib_config.h"

#include <Utility/INoCopyAndMove.h>

namespace ph::editor
{

class RTRScene;

class RenderThread : 
	public TFrameWorkerThread<
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES, 
		void(RTRScene&)>
{
public:
	

private:
};

}// end namespace ph::editor
