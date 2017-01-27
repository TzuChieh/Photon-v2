#pragma once

#include "Utility/TStableIndexDenseArray.h"

#include <memory>

namespace ph
{

class Frame;
class Renderer;
class Description;
class Film;

class ApiDatabase final
{
public:
	static std::size_t addRenderer(std::unique_ptr<Renderer> renderer);
	static bool removeRenderer(const std::size_t rendererId);
	static Renderer* getRenderer(const std::size_t rendererId);

	static std::size_t addFrame(std::unique_ptr<Frame> frame);
	static bool removeFrame(const std::size_t frameId);
	static Frame* getFrame(const std::size_t frameId);

	static std::size_t addDescription(std::unique_ptr<Description> description);
	static bool removeDescription(const std::size_t descriptionId);
	static Description* getDescription(const std::size_t descriptionId);

	static void releaseAllData();

private:
	static TStableIndexDenseArray<std::unique_ptr<Renderer>>    renderers;
	static TStableIndexDenseArray<std::unique_ptr<Frame>>       frames;
	static TStableIndexDenseArray<std::unique_ptr<Description>> descriptions;
};

}// end namespace ph