#include "Api/ApiDatabase.h"
#include "Frame/TFrame.h"
#include "Core/Engine.h"
#include "Common/Logger.h"

#include <utility>
#include <iostream>

namespace ph
{

namespace
{
	const Logger logger(LogSender("API Database"));
}

TStableIndexDenseArray<std::shared_ptr<Engine>>& ApiDatabase::ENGINES()
{
	static TStableIndexDenseArray<std::shared_ptr<Engine>> engines;
	return engines;
}

TStableIndexDenseArray<std::shared_ptr<HdrRgbFrame>>& ApiDatabase::FRAMES()
{
	static TStableIndexDenseArray<std::shared_ptr<HdrRgbFrame>> frames;
	return frames;
}

std::mutex& ApiDatabase::MUTEX()
{
	static std::mutex lock;
	return lock;
}

std::size_t ApiDatabase::addEngine(std::unique_ptr<Engine> engine)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return ENGINES().add(std::move(engine));
}

bool ApiDatabase::removeEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return ENGINES().remove(engineId);
}

Engine* ApiDatabase::getEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* engine = ENGINES().get(engineId);
	if(!engine)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"Engine<" + std::to_string(engineId) + "> does not exist");
		return nullptr;
	}

	return engine->get();
}

std::weak_ptr<Engine> ApiDatabase::useEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return ENGINES().isStableIndexValid(engineId) ? ENGINES()[engineId] : nullptr;
}

std::size_t ApiDatabase::addFrame(std::unique_ptr<HdrRgbFrame> frame)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return FRAMES().add(std::move(frame));
}

bool ApiDatabase::removeFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return FRAMES().remove(frameId);
}

HdrRgbFrame* ApiDatabase::getFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* frame = FRAMES().get(frameId);
	if(!frame)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Frame<" + std::to_string(frameId) + "> does not exist");
		return nullptr;
	}

	return frame->get();
}

std::weak_ptr<HdrRgbFrame> ApiDatabase::useFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return FRAMES().isStableIndexValid(frameId) ? FRAMES()[frameId] : nullptr;
}

void ApiDatabase::clear()
{
	std::lock_guard<std::mutex> lock(MUTEX());

	ENGINES().removeAll();
	FRAMES().removeAll();
}

}// end namespace ph