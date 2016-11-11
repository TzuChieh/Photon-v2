#pragma once

#include "Utility/TStableIndexDenseArray.h"
#include "Core/Renderer.h"

#include <memory>

namespace ph
{

class HDRFrame;
class RenderTask;

class ApiDatabase final
{
public:
	static std::size_t addRenderTask(std::unique_ptr<RenderTask> renderTask);
	static bool removeRenderTask(const std::size_t renderTaskId);

	static std::size_t addRenderer(std::unique_ptr<Renderer> renderer);
	static bool removeRenderer(const std::size_t rendererId);

	static std::size_t addHdrFrame(std::unique_ptr<HDRFrame> hdrFrame);
	static bool removeHdrFrame(const std::size_t frameId);

	static void releaseAllData();

private:
	static TStableIndexDenseArray<std::unique_ptr<RenderTask>> renderTasks;
	static TStableIndexDenseArray<std::unique_ptr<Renderer>>   renderers;
	static TStableIndexDenseArray<std::unique_ptr<HDRFrame>>   hdrFrames;
};

}// end namespace ph