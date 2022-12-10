#include "App/Misc/EditorStats.h"

namespace ph::editor
{

EditorStats::EditorStats()
	: mainThreadUpdateMs(0)
	, mainThreadRenderMs(0)
	, mainThreadEventFlushMs(0)
	, mainThreadFrameMs(0)
	, renderThreadFrameMs(0)
	, ghiThreadFrameMs(0)
{}

}// end namespace ph::editor
