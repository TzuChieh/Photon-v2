#pragma once

#include "Utility/TStableIndexDenseArray.h"

#include <memory>

namespace ph
{

class Frame;
class Engine;

class ApiDatabase final
{

	// TODO: use lazy initialization

public:
	static std::size_t addEngine(std::unique_ptr<Engine> engine);
	static bool removeEngine(const std::size_t engineId);
	static Engine* getEngine(const std::size_t engineId);

	static std::size_t addFrame(std::unique_ptr<Frame> frame);
	static bool removeFrame(const std::size_t frameId);
	static Frame* getFrame(const std::size_t frameId);

private:
	static TStableIndexDenseArray<std::unique_ptr<Engine>> engines;
	static TStableIndexDenseArray<std::unique_ptr<Frame>>  frames;

	friend bool exit_api_database();
	static void clear();
};

}// end namespace ph