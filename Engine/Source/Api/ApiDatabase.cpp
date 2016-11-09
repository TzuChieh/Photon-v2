#include "Api/ApiDatabase.h"
#include "Image/HDRFrame.h"

#include <utility>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Renderer>> ApiDatabase::renderers;
TStableIndexDenseArray<HDRFrame> ApiDatabase::hdrFrames;

std::size_t ApiDatabase::addRenderer(std::unique_ptr<Renderer> renderer)
{
	return renderers.add(std::move(renderer));
}

bool ApiDatabase::removeRenderer(const std::size_t rendererIndex)
{
	return renderers.remove(rendererIndex);
}

std::size_t ApiDatabase::addHdrFrame(HDRFrame&& hdrFrame)
{
	return hdrFrames.add(std::move(hdrFrame));
}

bool ApiDatabase::removeHdrFrame(const std::size_t frameIndex)
{
	return hdrFrames.remove(frameIndex);
}

void ApiDatabase::releaseAllData()
{
	hdrFrames.removeAll();
}

}// end namespace ph