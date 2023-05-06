#pragma once

#include "Utility/TSpan.h"

#include <string_view>
#include <cstddef>

namespace ph
{

class SdlClass;
class ISdlResource;

class SdlDataPacketParser
{
public:
	explicit SdlDataPacketParser(TSpanView<const SdlClass*> targetClasses);
	virtual ~SdlDataPacketParser();

	virtual bool beingPacket(const SdlClass* targetClass) = 0;
	virtual ISdlResource* createResource(const SdlClass* targetClass) = 0;
	virtual void storeResource(ISdlResource* resource) = 0;
	virtual bool endPacket() = 0;

	void parse(std::string_view packetStr);
	void parse(TSpanView<std::byte> packetBytes);

private:
	TSpanView<const SdlClass*> m_targetClasses;
};

}// end namespace ph
