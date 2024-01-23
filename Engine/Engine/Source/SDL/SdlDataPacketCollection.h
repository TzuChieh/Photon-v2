#pragma once

#include "SDL/ISdlDataPacketGroup.h"
#include "SDL/SdlInputClauses.h"

#include <Utility/string_utils.h>

#include <unordered_map>

namespace ph
{

class SdlDataPacketCollection : public ISdlDataPacketGroup
{
public:
	const SdlInputClauses* get(std::string_view packetName) const override;

	void add(
		SdlInputClauses packet,
		std::string_view packetName);

private:
	using PacketMapType = string_utils::TStdUnorderedStringMap<SdlInputClauses>;

	PacketMapType m_nameToPacket;
};

inline const SdlInputClauses* SdlDataPacketCollection::get(std::string_view packetName) const
{
	const auto& iter = m_nameToPacket.find(packetName);
	return iter != m_nameToPacket.end() ? &(iter->second) : nullptr;
}

}// end namespace ph
