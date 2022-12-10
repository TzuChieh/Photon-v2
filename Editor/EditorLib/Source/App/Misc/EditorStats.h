#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

class EditorStats final
{
public:
	float32 mainThreadUpdateMs;
	float32 mainThreadRenderMs;
	float32 mainThreadEventFlushMs;
	float32 mainThreadFrameMs;
	float32 renderThreadFrameMs;
	float32 ghiThreadFrameMs;

public:
	EditorStats();

private:
};

}// end namespace ph::editor
