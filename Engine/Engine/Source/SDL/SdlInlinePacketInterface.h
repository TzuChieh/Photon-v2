#pragma once

#include "SDL/SdlDataPacketInterface.h"
#include "SDL/sdl_fwd.h"
#include "Utility/TSpan.h"

#include <string>
#include <vector>

namespace ph
{

/*! @brief Supports the case where the packet command stores SDL clauses directly.
*/
class SdlInlinePacketInterface : public SdlDataPacketInterface
{
public:
	SdlInlinePacketInterface();

	/*!
	@param clauseIndentAmount Controls how much to indent each clause during clause generation.
	Possible values:
	-1 = No indentation. All clauses are on the same line.
	0 = No indentation. Each clause will be placed on a new line.
	>0 = Indent each clause by the specified amount of `clauseIndentChar`.
	@param clauseIndentChar Character used for indentation during clause generation. Must be a whitespace.
	*/
	SdlInlinePacketInterface(int clauseIndentAmount, char clauseIndentChar);

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
	static void parseClauses(
		TSpanView<std::string> clauseStrings,
		const SdlInputContext& ctx, 
		SdlInputClauses& out_clauses);

	static void parseSingleClause(
		TSpanView<std::string> clauseTokens,
		SdlInputClause& out_clause);

	static void appendSingleClause(
		const SdlOutputClause& clause,
		std::string& out_commandStr);

	std::string m_clausePrefix;
};

}// end namespace ph
