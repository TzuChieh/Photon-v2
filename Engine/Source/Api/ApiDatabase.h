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
	static std::size_t                addEngine(std::unique_ptr<Engine> engine);
	static bool                       removeEngine(std::size_t engineId);
	static Engine*                    getEngine(std::size_t engineId);
	static std::weak_ptr<Engine>      useEngine(std::size_t engineId);

	static std::size_t                addFrame(std::unique_ptr<HdrRgbFrame> frame);
	static bool                       removeFrame(std::size_t frameId);
	static HdrRgbFrame*               getFrame(std::size_t frameId);
	static std::weak_ptr<HdrRgbFrame> useFrame(std::size_t frameId);

private:
	static std::mutex& MUTEX();

	static TStableIndexDenseArray<std::shared_ptr<Engine>>&      ENGINES();
	static TStableIndexDenseArray<std::shared_ptr<HdrRgbFrame>>& FRAMES();

	static void clear();
};

}// end namespace ph