#pragma once

#include "DataIO/SDL/InputPacket.h"
#include "DataIO/SDL/DataTreatment.h"

#include <cstddef>
#include <string_view>
#include <variant>
#include <type_traits>

namespace ph
{

class PacketDataRetriever final
{
public:
	explicit PacketDataRetriever(const InputPacket& packet);

	/*template<typename DataType = std::nullptr_t, typename... RemainingDataTypes>
	bool get(
		std::string_view dataName,
		std::variant<DataType, RemainingDataTypes...>& data,
		const DataTreatment& treatment = DataTreatment()) const;*/

private:
	const InputPacket& m_packet;
};

// In-header Implementations:

inline PacketDataRetriever::PacketDataRetriever(const InputPacket& packet) : 
	m_packet(packet)
{}

//template<typename DataType, typename... RemainingDataTypes>
//inline bool PacketDataRetriever::get(
//	const std::string_view dataName,
//	std::variant<DataType, RemainingDataTypes...>& data,
//	const DataTreatment& treatment) const
//{
//	if constexpr(std::is_same_v<DataType, std::nullptr_t>)
//	{
//		return false;
//	}
//}

}// end namespace ph
