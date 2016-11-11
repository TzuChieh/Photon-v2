#include "Api/ApiDatabase.h"
#include "Image/HDRFrame.h"
#include "Core/RenderTask.h"
#include "Core/Renderer.h"
#include "Camera/Camera.h"
#include "World/World.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<RenderTask>> ApiDatabase::renderTasks;
TStableIndexDenseArray<std::unique_ptr<Renderer>>   ApiDatabase::renderers;
TStableIndexDenseArray<std::unique_ptr<HDRFrame>>   ApiDatabase::hdrFrames;
TStableIndexDenseArray<std::unique_ptr<World>>      ApiDatabase::worlds;
TStableIndexDenseArray<std::unique_ptr<Camera>>     ApiDatabase::cameras;

// ***************************************************************************
// RenderTask

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

// ***************************************************************************
// Renderer

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

// ***************************************************************************
// Frame

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

// ***************************************************************************
// World

std::size_t ApiDatabase::addWrold(std::unique_ptr<World> world)
{
	return worlds.add(std::move(world));
}

bool ApiDatabase::removeWrold(const std::size_t worldId)
{
	return worlds.remove(worldId);
}

World* ApiDatabase::getWrold(const std::size_t worldId)
{
	World* world = worlds.get(worldId)->get();
	if(world == nullptr)
	{
		std::cerr << "World<" + worldId << "> does not exist" << std::endl;
	}

	return world;
}

// ***************************************************************************
// Camera

std::size_t ApiDatabase::addCamera(std::unique_ptr<Camera> camera)
{
	return cameras.add(std::move(camera));
}

bool ApiDatabase::removeCamera(const std::size_t cameraId)
{
	return cameras.remove(cameraId);
}

Camera* ApiDatabase::getCamera(const std::size_t cameraId)
{
	Camera* camera = cameras.get(cameraId)->get();
	if(camera == nullptr)
	{
		std::cerr << "Camera<" + cameraId << "> does not exist" << std::endl;
	}

	return camera;
}

void ApiDatabase::releaseAllData()
{
	hdrFrames.removeAll();
}

}// end namespace ph