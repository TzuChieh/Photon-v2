#pragma once

#include "Utility/TStableIndexDenseArray.h"
#include "Frame/TFrame.h"

#include <memory>
#include <mutex>

namespace ph
{

class Engine;

class ApiDatabase final
{
	friend bool exit_api_database();

public:
	static std::size_t addEngine(std::unique_ptr<Engine> engine);
	static bool        removeEngine(const std::size_t engineId);
	static Engine*     getEngine(const std::size_t engineId);

	static std::size_t  addFrame(std::unique_ptr<HdrRgbFrame> frame);
	static bool         removeFrame(const std::size_t frameId);
	static HdrRgbFrame* getFrame(const std::size_t frameId);

private:
	static std::mutex& MUTEX_LOCK();

	static TStableIndexDenseArray<std::unique_ptr<Engine>>&      ENGINES();
	static TStableIndexDenseArray<std::unique_ptr<HdrRgbFrame>>& FRAMES();

	static void clear();
};

}// end namespace ph