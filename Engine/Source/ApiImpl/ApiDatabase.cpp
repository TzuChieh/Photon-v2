#include "ApiImpl/ApiDatabase.h"
#include "Frame/HDRFrame.h"

#include <utility>

namespace ph
{

TStableIndexDenseArray<HDRFrame> ApiDatabase::hdrFrames;

std::size_t ApiDatabase::addHdrFrame(HDRFrame&& hdrFrame)
{
	return hdrFrames.add(std::move(hdrFrame));
}

bool ApiDatabase::removeHdrFrame(const std::size_t frameIndex)
{
	return hdrFrames.remove(frameIndex);
}

}// end namespace ph