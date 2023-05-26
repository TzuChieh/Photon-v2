#pragma once

#include "SDL/SdlDataPacketGenerator.h"

namespace ph
{

class SdlOutputClause;

/*! @brief Generating a standalone collection of SDL clauses.
*/
class SdlInlinePacketGenerator : public SdlDataPacketGenerator
{
public:
	SdlInlinePacketGenerator();
	explicit SdlInlinePacketGenerator(Path sceneWorkingDirectory);

	void generate(
		const SdlOutputClauses& clauses,
		std::string& out_packetCommand) const override;

private:
	static void appendClause(
		const SdlOutputClause& clause,
		std::string& out_commandStr);
};

}// end namespace ph
