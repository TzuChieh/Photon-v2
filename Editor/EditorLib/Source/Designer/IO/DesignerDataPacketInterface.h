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
	explicit DesignerDataPacketInterface(const Path& sceneWorkingDirectory);

	void parse(
		std::string_view packetCommand,
		const SdlClass* targetClass,
		std::string_view targetName,
		SdlInputClauses& out_clauses) const override;

	void generate(
		const SdlOutputClauses& clauses,
		const SdlClass* targetClass,
		std::string_view targetName,
		std::string& out_packetCommand) const override;

private:
	static void parseTextPacket(const std::string& text, SdlInputClauses& out_clauses);

	static void parseSingleClause(
		std::string_view valueInfoStr,
		std::string_view valueStr,
		SdlInputClause& out_clause);
};

}// end namespace ph::editor
