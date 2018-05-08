#pragma once

#include "Common/primitive_type.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

class RenderOption final
{
public:
	uint32 numThreads;

	inline RenderOption() : 
		numThreads(1)
	{}

	inline RenderOption(const InputPacket& packet) : 
		RenderOption()
	{
		numThreads = static_cast<uint32>(packet.getInteger("threads", numThreads, DataTreatment(EDataImportance::REQUIRED)));
	}
};

}// end namespace ph