#include "Api/ApiDatabase.h"
#include "Image/HDRFrame.h"
#include "Core/RenderTask.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<RenderTask>> ApiDatabase::renderTasks;
TStableIndexDenseArray<std::unique_ptr<Renderer>>   ApiDatabase::renderers;
TStableIndexDenseArray<std::unique_ptr<HDRFrame>>   ApiDatabase::hdrFrames;

std::size_t ApiDatabase::addRenderTask(std::unique_ptr<RenderTask> renderTask)
{
	return renderTasks.add(std::move(renderTask));
}

bool ApiDatabase::removeRenderTask(const std::size_t renderTaskId)
{
	return renderTasks.remove(renderTaskId);
}

RenderTask* ApiDatabase::getRenderTask(const std::size_t renderTaskId)
{
	RenderTask* renderTask = renderTasks.get(renderTaskId)->get();
	if(renderTask == nullptr)
	{
		std::cerr << "RenderTesk<" + renderTaskId << "> does not exist" << std::endl;
	}

	return renderTask;
}

std::size_t ApiDatabase::addRenderer(std::unique_ptr<Renderer> renderer)
{
	return renderers.add(std::move(renderer));
}

bool ApiDatabase::removeRenderer(const std::size_t rendererId)
{
	return renderers.remove(rendererId);
}

Renderer* ApiDatabase::getRenderer(const std::size_t rendererId)
{
	Renderer* renderer = renderers.get(rendererId)->get();
	if(renderer == nullptr)
	{
		std::cerr << "Renderer<" + rendererId << "> does not exist" << std::endl;
	}

	return renderer;
}

std::size_t ApiDatabase::addHdrFrame(std::unique_ptr<HDRFrame> hdrFrame)
{
	return hdrFrames.add(std::move(hdrFrame));
}

bool ApiDatabase::removeHdrFrame(const std::size_t frameId)
{
	return hdrFrames.remove(frameId);
}

HDRFrame* ApiDatabase::getHdrFrame(const std::size_t frameId)
{
	HDRFrame* frame = hdrFrames.get(frameId)->get();
	if(frame == nullptr)
	{
		std::cerr << "Frame<" + frameId << "> does not exist" << std::endl;
	}

	return frame;
}

void ApiDatabase::releaseAllData()
{
	hdrFrames.removeAll();
}

}// end namespace ph