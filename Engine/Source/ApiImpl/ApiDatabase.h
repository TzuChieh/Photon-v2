#pragma once

#include "Utility/TStableIndexDenseArray.h"

namespace ph
{

class HDRFrame;

class ApiDatabase final
{
public:
	static std::size_t addHdrFrame(HDRFrame&& hdrFrame);
	static bool removeHdrFrame(const std::size_t frameIndex);

private:
	static TStableIndexDenseArray<HDRFrame> hdrFrames;
};

}// end namespace ph