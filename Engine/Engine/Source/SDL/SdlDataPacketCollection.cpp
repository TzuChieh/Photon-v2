#include "SDL/SdlDataPacketCollection.h"
#include "SDL/sdl_exceptions.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlDataPacketCollection, SDL);

void SdlDataPacketCollection::addUnique(
	SdlInputClauses packet,
	std::string_view packetName)
{
	const auto& iter = m_nameToPacket.find(packetName);
	if(iter != m_nameToPacket.end())
	{
		PH_LOG(SdlDataPacketCollection, Warning,
			"duplicated SDL data packet detected, overwriting (name: {})", packetName);
	}

	add(std::move(packet), packetName);
}

void SdlDataPacketCollection::addOrUpdate(
	SdlInputClauses packet,
	std::string_view packetName)
{
	add(std::move(packet), packetName);
}

void SdlDataPacketCollection::add(
	SdlInputClauses packet,
	std::string_view packetName)
{
	if(packetName.empty())
	{
		throw SdlLoadError(
			"cannot add SDL data packet due to empty name");
	}

	m_nameToPacket[std::string(packetName)] = std::move(packet);
}

}// end namespace ph
