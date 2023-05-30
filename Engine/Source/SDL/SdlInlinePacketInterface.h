#pragma once

#include "SDL/SdlDataPacketInterface.h"

namespace ph
{

class SdlInputClause;
class SdlInputClauses;
class SdlOutputClause;
class SdlOutputClauses;

/*! @brief Supports the case where the packet command stores SDL clauses directly.
*/
class SdlInlinePacketInterface : public SdlDataPacketInterface
{
public:
	SdlInlinePacketInterface();
	explicit SdlInlinePacketInterface(const Path& sceneWorkingDirectory);

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
	static void parseClauses(std::string_view packetCommand, SdlInputClauses& out_clauses);
	static void parseSingleClause(std::string_view clauseString, SdlInputClause& out_clause);

	static void appendSingleClause(
		const SdlOutputClause& clause,
		std::string& out_commandStr);
};

}// end namespace ph
