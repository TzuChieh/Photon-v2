#include "Api/ApiDatabase.h"
#include "Image/Frame.h"
#include "Core/RenderTask.h"
#include "Core/Renderer.h"
#include "Camera/Camera.h"
#include "World/World.h"
#include "Core/SampleGenerator.h"
#include "Image/Film.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<RenderTask>>      ApiDatabase::renderTasks;
TStableIndexDenseArray<std::unique_ptr<Renderer>>        ApiDatabase::renderers;
TStableIndexDenseArray<std::unique_ptr<Frame>>           ApiDatabase::frames;
TStableIndexDenseArray<std::unique_ptr<World>>           ApiDatabase::worlds;
TStableIndexDenseArray<std::unique_ptr<Camera>>          ApiDatabase::cameras;
TStableIndexDenseArray<std::unique_ptr<SampleGenerator>> ApiDatabase::sampleGenerators;
TStableIndexDenseArray<std::unique_ptr<Film>>            ApiDatabase::films;

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

std::size_t ApiDatabase::addFrame(std::unique_ptr<Frame> frame)
{
	return frames.add(std::move(frame));
}

bool ApiDatabase::removeFrame(const std::size_t frameId)
{
	return frames.remove(frameId);
}

Frame* ApiDatabase::getFrame(const std::size_t frameId)
{
	Frame* frame = frames.get(frameId)->get();
	if(frame == nullptr)
	{
		std::cerr << "Frame<" + frameId << "> does not exist" << std::endl;
	}

	return frame;
}

// ***************************************************************************
// World

std::size_t ApiDatabase::addWorld(std::unique_ptr<World> world)
{
	return worlds.add(std::move(world));
}

bool ApiDatabase::removeWorld(const std::size_t worldId)
{
	return worlds.remove(worldId);
}

World* ApiDatabase::getWorld(const std::size_t worldId)
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

// ***************************************************************************
// SampleGenerator

std::size_t ApiDatabase::addSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	return sampleGenerators.add(std::move(sampleGenerator));
}

bool ApiDatabase::removeSampleGenerator(const std::size_t sampleGeneratorId)
{
	return sampleGenerators.remove(sampleGeneratorId);
}

SampleGenerator* ApiDatabase::getSampleGenerator(const std::size_t sampleGeneratorId)
{
	SampleGenerator* sampleGenerator = sampleGenerators.get(sampleGeneratorId)->get();
	if(sampleGenerator == nullptr)
	{
		std::cerr << "SampleGenerator<" + sampleGeneratorId << "> does not exist" << std::endl;
	}

	return sampleGenerator;
}

// ***************************************************************************
// Film

std::size_t ApiDatabase::addFilm(std::unique_ptr<Film> film)
{
	return films.add(std::move(film));
}

bool ApiDatabase::removeFilm(const std::size_t filmId)
{
	return films.remove(filmId);
}

Film* ApiDatabase::getFilm(const std::size_t filmId)
{
	Film* film = films.get(filmId)->get();
	if(film == nullptr)
	{
		std::cerr << "Film<" + filmId << "> does not exist" << std::endl;
	}

	return film;
}

void ApiDatabase::releaseAllData()
{
	frames.removeAll();
	renderTasks.removeAll();
	renderers.removeAll();
	worlds.removeAll();
	cameras.removeAll();
	sampleGenerators.removeAll();
	films.removeAll();
}

}// end namespace ph