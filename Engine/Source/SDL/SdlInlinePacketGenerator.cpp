#include "SDL/SdlInlinePacketGenerator.h"
#include "SDL/SdlOutputClause.h"
#include "SDL/SdlOutputClauses.h"

#include <utility>

namespace ph
{

SdlInlinePacketGenerator::SdlInlinePacketGenerator()
	: SdlInlinePacketGenerator(Path("./"))
{}

SdlInlinePacketGenerator::SdlInlinePacketGenerator(Path sceneWorkingDirectory)
	: SdlDataPacketGenerator(std::move(sceneWorkingDirectory))
{}

void SdlInlinePacketGenerator::generate(
	const SdlOutputClauses& clauses,
	std::string& out_packetCommand) const
{
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.numClauses(); ++clauseIdx)
	{
		appendClause(clauses[clauseIdx], out_packetCommand);
	}
}

void SdlInlinePacketGenerator::appendClause(
	const SdlOutputClause& clause,
	std::string& out_commandStr)
{
	out_commandStr += '[';
	out_commandStr += clause.type;
	out_commandStr += ' ';
	out_commandStr += clause.name;

	if(clause.hasTag())
	{
		out_commandStr += ": ";
		out_commandStr += clause.tag;
	}

	out_commandStr += ' ';
	out_commandStr += clause.value;
	out_commandStr += ']';
}

}// end namespace ph
