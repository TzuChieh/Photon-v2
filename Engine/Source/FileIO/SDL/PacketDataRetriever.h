#pragma once

#include "FileIO/SDL/InputPacket.h"

#include <variant>

namespace ph
{

class PacketDataRetriever final
{
public:
	explicit PacketDataRetriever(const InputPacket& packet);

private:
	const InputPacket& m_packet;
};

// In-header Implementations:

inline PacketDataRetriever::PacketDataRetriever(const InputPacket& packet) : 
	m_packet(packet)
{}

}// end namespace ph