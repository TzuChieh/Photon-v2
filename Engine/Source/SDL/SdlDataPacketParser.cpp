#include "SDL/SdlDataPacketParser.h"

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
	// create a JSON value
	//json j = R"({"compact": true, "schema": 0})"_json;

	//// serialize to BSON
	//std::vector<std::uint8_t> v_bson = json::to_bson(j);
	////std::vector<std::byte> v_bson = json::to_bson(j);
	//std::vector<std::byte> bytes;
	//for(std::uint8_t b : v_bson)
	//{
	//	bytes.push_back(static_cast<std::byte>(b));
	//}

	//// 0x1B, 0x00, 0x00, 0x00, 0x08, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x00, 0x01, 0x10, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

	//// roundtrip
	//json j_from_bson = json::from_bson(bytes);
}

}// end namespace ph
