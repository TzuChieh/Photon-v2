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
	static void parseClauses(std::string_view packetCommand, SdlInputClauses& out_clauses);
	static void parseSingleClause(std::string_view clauseString, SdlInputClause& out_clause);

	static void appendSingleClause(
		const SdlOutputClause& clause,
		std::string& out_commandStr);
};

}// end namespace ph
