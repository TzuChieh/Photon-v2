#pragma once

#include "Utility/TStableIndexDenseArray.h"

#include <memory>
#include <mutex>

namespace ph
{

template<typename ComponentType>
class TFrame;

class Engine;

class ApiDatabase final
{
	friend bool exit_api_database();

public:
	static std::size_t addEngine(std::unique_ptr<Engine> engine);
	static bool removeEngine(const std::size_t engineId);
	static Engine* getEngine(const std::size_t engineId);

	static std::size_t addFrame(std::unique_ptr<TFrame<real>> frame);
	static bool removeFrame(const std::size_t frameId);
	static TFrame<real>* getFrame(const std::size_t frameId);

private:
	static std::mutex& MUTEX_LOCK();

	static TStableIndexDenseArray<std::unique_ptr<Engine>>&       ENGINES();
	static TStableIndexDenseArray<std::unique_ptr<TFrame<real>>>& FRAMES();

	static void clear();
};

}// end namespace ph