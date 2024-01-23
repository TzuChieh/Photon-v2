#include "SDL/SdlDataPacketCollection.h"
#include "SDL/sdl_exceptions.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlDataPacketCollection, SDL);

void SdlDataPacketCollection::add(
	SdlInputClauses packet,
	std::string_view packetName)
{
	if(packetName.empty())
	{
		throw_formatted<SdlLoadError>(
			"cannot add SDL data packet due to empty name (name: {})", 
			packetName.empty() ? packetName : "(no name)");
	}

	const auto& iter = m_nameToPacket.find(packetName);
	if(iter != m_nameToPacket.end())
	{
		PH_LOG_WARNING(SdlDataPacketCollection,
			"duplicated SDL data packet detected, overwriting (name: {})", packetName);
	}

	m_nameToPacket[std::string(packetName)] = std::move(packet);
}

}// end namespace ph
