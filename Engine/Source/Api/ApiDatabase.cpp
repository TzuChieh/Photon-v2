#include "Api/ApiDatabase.h"
#include "Filmic/Frame.h"
#include "Core/Renderer.h"
#include "Camera/Camera.h"
#include "World/World.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Filmic/Film.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Renderer>>        ApiDatabase::renderers;
TStableIndexDenseArray<std::unique_ptr<Frame>>           ApiDatabase::frames;
TStableIndexDenseArray<std::unique_ptr<World>>           ApiDatabase::worlds;
TStableIndexDenseArray<std::unique_ptr<Camera>>          ApiDatabase::cameras;
TStableIndexDenseArray<std::unique_ptr<SampleGenerator>> ApiDatabase::sampleGenerators;
TStableIndexDenseArray<std::unique_ptr<Film>>            ApiDatabase::films;

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
	auto* renderer = renderers.get(rendererId);
	if(renderer == nullptr)
	{
		std::cerr << "Renderer<" << rendererId << "> does not exist" << std::endl;
		return nullptr;
	}

	return renderer->get();
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
	auto* frame = frames.get(frameId);
	if(frame == nullptr)
	{
		std::cerr << "Frame<" << frameId << "> does not exist" << std::endl;
		return nullptr;
	}

	return frame->get();
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
	auto* world = worlds.get(worldId);
	if(world == nullptr)
	{
		std::cerr << "World<" << worldId << "> does not exist" << std::endl;
		return nullptr;
	}

	return world->get();
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
	auto* camera = cameras.get(cameraId);
	if(camera == nullptr)
	{
		std::cerr << "Camera<" << cameraId << "> does not exist" << std::endl;
		return nullptr;
	}

	return camera->get();
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
	auto* sampleGenerator = sampleGenerators.get(sampleGeneratorId);
	if(sampleGenerator == nullptr)
	{
		std::cerr << "SampleGenerator<" << sampleGeneratorId << "> does not exist" << std::endl;
		return nullptr;
	}

	return sampleGenerator->get();
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
	auto* film = films.get(filmId);
	if(film == nullptr)
	{
		std::cerr << "Film<" << filmId << "> does not exist" << std::endl;
		return nullptr;
	}

	return film->get();
}

void ApiDatabase::releaseAllData()
{
	frames.removeAll();
	renderers.removeAll();
	worlds.removeAll();
	cameras.removeAll();
	sampleGenerators.removeAll();
	films.removeAll();
}

}// end namespace ph