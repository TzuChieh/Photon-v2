#pragma once

#include "SDL/ISdlDataPacketGroup.h"
#include "SDL/SdlInputClauses.h"

#include <Container/TStdUnorderedStringMap.h>

#include <unordered_map>

namespace ph
{

class SdlDataPacketCollection : public ISdlDataPacketGroup
{
public:
	const SdlInputClauses* get(std::string_view packetName) const override;

	void addUnique(
		SdlInputClauses packet,
		std::string_view packetName);

	void addOrUpdate(
		SdlInputClauses packet,
		std::string_view packetName);

private:
	void add(
		SdlInputClauses packet,
		std::string_view packetName);

	using PacketMapType = TStdUnorderedStringMap<SdlInputClauses>;

	PacketMapType m_nameToPacket;
};

inline const SdlInputClauses* SdlDataPacketCollection::get(std::string_view packetName) const
{
	const auto& iter = m_nameToPacket.find(packetName);
	return iter != m_nameToPacket.end() ? &(iter->second) : nullptr;
}

}// end namespace ph
