#include "Api/ApiDatabase.h"
#include "PostProcess/Frame.h"
#include "Core/Engine.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Engine>>& ApiDatabase::ENGINES()
{
	static TStableIndexDenseArray<std::unique_ptr<Engine>> engines;
	return engines;
}

TStableIndexDenseArray<std::unique_ptr<Frame>>& ApiDatabase::FRAMES()
{
	static TStableIndexDenseArray<std::unique_ptr<Frame>> frames;
	return frames;
}

std::mutex& ApiDatabase::MUTEX_LOCK()
{
	static std::mutex lock;
	return lock;
}

std::size_t ApiDatabase::addEngine(std::unique_ptr<Engine> engine)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	return ENGINES().add(std::move(engine));
}

bool ApiDatabase::removeEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	return ENGINES().remove(engineId);
}

Engine* ApiDatabase::getEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	auto* engine = ENGINES().get(engineId);
	if(engine == nullptr)
	{
		std::cerr << "Engine<" << engineId << "> does not exist" << std::endl;
		return nullptr;
	}

	return engine->get();
}

std::size_t ApiDatabase::addFrame(std::unique_ptr<Frame> frame)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	return FRAMES().add(std::move(frame));
}

bool ApiDatabase::removeFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	return FRAMES().remove(frameId);
}

Frame* ApiDatabase::getFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	auto* frame = FRAMES().get(frameId);
	if(frame == nullptr)
	{
		std::cerr << "Frame<" << frameId << "> does not exist" << std::endl;
		return nullptr;
	}

	return frame->get();
}

void ApiDatabase::clear()
{
	std::lock_guard<std::mutex> lock(MUTEX_LOCK());

	ENGINES().removeAll();
	FRAMES().removeAll();
}

}// end namespace ph