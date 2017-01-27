#pragma once

#include "Common/primitive_type.h"
#include "FileIO/InputPacket.h"

namespace ph
{

class RenderOption final
{
public:
	uint32 numThreads;

	inline RenderOption() : 
		numThreads(1)
	{

	}

	inline RenderOption(const InputPacket& packet) : 
		RenderOption()
	{
		numThreads = static_cast<uint32>(packet.getInteger("threads", numThreads, "RenderOption >> argument threads not found"));
	}
};

}// end namespace ph