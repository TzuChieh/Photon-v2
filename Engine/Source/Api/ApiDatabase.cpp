#include "Api/ApiDatabase.h"
#include "PostProcess/Frame.h"
#include "Core/Renderer.h"
#include "FileIO/Description.h"
#include "Core/Camera/Film.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Renderer>>    ApiDatabase::renderers;
TStableIndexDenseArray<std::unique_ptr<Frame>>       ApiDatabase::frames;
TStableIndexDenseArray<std::unique_ptr<Description>> ApiDatabase::descriptions;

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
// Description

std::size_t ApiDatabase::addDescription(std::unique_ptr<Description> description)
{
	return descriptions.add(std::move(description));
}

bool ApiDatabase::removeDescription(const std::size_t descriptionId)
{
	return descriptions.remove(descriptionId);
}

Description* ApiDatabase::getDescription(const std::size_t descriptionId)
{
	auto* description = descriptions.get(descriptionId);
	if(description == nullptr)
	{
		std::cerr << "Description<" << descriptionId << "> does not exist" << std::endl;
		return nullptr;
	}

	return description->get();
}

void ApiDatabase::releaseAllData()
{
	frames.removeAll();
	renderers.removeAll();
	descriptions.removeAll();
}

}// end namespace ph