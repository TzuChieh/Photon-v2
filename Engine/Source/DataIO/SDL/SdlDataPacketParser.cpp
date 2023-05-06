#include "DataIO/SDL/SdlDataPacketParser.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace ph
{

SdlDataPacketParser::SdlDataPacketParser(TSpanView<const SdlClass*> targetClasses)
	: m_targetClasses(targetClasses)
{}

SdlDataPacketParser::~SdlDataPacketParser() = default;

void SdlDataPacketParser::parse(std::string_view packetStr)
{
	// TODO
	auto j3 = json::parse(R"({"happy": true, "pi": 3.141})");
}

void SdlDataPacketParser::parse(TSpanView<std::byte> packetBytes)
{
	// TODO
}

}// end namespace ph
