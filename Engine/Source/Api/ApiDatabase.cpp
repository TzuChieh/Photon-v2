#include "Api/ApiDatabase.h"
#include "PostProcess/Frame.h"
#include "Core/Engine.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Engine>> ApiDatabase::engines;
TStableIndexDenseArray<std::unique_ptr<Frame>>  ApiDatabase::frames;

std::size_t ApiDatabase::addEngine(std::unique_ptr<Engine> engine)
{
	return engines.add(std::move(engine));
}

bool ApiDatabase::removeEngine(const std::size_t engineId)
{
	return engines.remove(engineId);
}

Engine* ApiDatabase::getEngine(const std::size_t engineId)
{
	auto* engine = engines.get(engineId);
	if(engine == nullptr)
	{
		std::cerr << "Engine<" << engineId << "> does not exist" << std::endl;
		return nullptr;
	}

	return engine->get();
}

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

void ApiDatabase::clear()
{
	engines.removeAll();
	frames.removeAll();
}

}// end namespace ph