#pragma once

#include "Utility/TStableIndexDenseArray.h"
#include "Core/Renderer.h"

#include <memory>

namespace ph
{

class HDRFrame;

class ApiDatabase final
{
public:
	static std::size_t addRenderer(std::unique_ptr<Renderer> renderer);
	static bool removeRenderer(const std::size_t rendererIndex);

	static std::size_t addHdrFrame(HDRFrame&& hdrFrame);
	static bool removeHdrFrame(const std::size_t frameIndex);

	static void releaseAllData();

private:
	static TStableIndexDenseArray<std::unique_ptr<Renderer>> renderers;
	static TStableIndexDenseArray<HDRFrame> hdrFrames;
};

}// end namespace ph