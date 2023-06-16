#pragma once

#include <SDL/SdlDataPacketInterface.h>

#include <string>

namespace ph { class SdlInputClause; }

namespace ph::editor
{

/*! @brief Data packet format for the editor scene.
*/
class DesignerDataPacketInterface : public SdlDataPacketInterface
{
public:
	DesignerDataPacketInterface();

	void parse(
		std::string_view packetCommand,
		const SdlInputContext& ctx,
		std::string_view targetName,
		ISdlResource* targetInstance,
		SdlInputClauses& out_clauses) const override;

	void generate(
		const SdlOutputClauses& clauses,
		const SdlOutputContext& ctx,
		std::string_view targetName,
		const ISdlResource* targetInstance,
		std::string& out_packetCommand) const override;

private:
	static void parseTextPacket(const std::string& text, SdlInputClauses& out_clauses);

	static void parseSingleClause(
		std::string_view valueInfoStr,
		std::string_view valueStr,
		SdlInputClause& out_clause);
};

}// end namespace ph::editor
