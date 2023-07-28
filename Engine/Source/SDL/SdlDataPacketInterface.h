#pragma once

#include "SDL/sdl_fwd.h"

#include <string>
#include <string_view>

namespace ph
{

/*! @brief Parsing & generating a group of SDL clauses.
*/
class SdlDataPacketInterface
{
public:
	SdlDataPacketInterface();
	virtual ~SdlDataPacketInterface();

	/*! @brief Turn a packet command into @p out_clauses.
	@param targetClass The class current packet is being parsed for. Null if unavailable.
	@param targetName The target's name current packet is being parsed for. Empty if unavailable.
	@param targetInstance Pointer to the instance current packet is being parsed for. Null if unavailable.
	@param[out] out_clauses Parsed clauses. Results are appended and will be used for initializing the
	target instance.
	*/
	virtual void parse(
		std::string_view packetCommand,
		const SdlInputContext& ctx,
		std::string_view targetName,
		ISdlResource* targetInstance,
		SdlInputClauses& out_clauses) const = 0;

	/*! @brief Turn all data in the @p clauses into a packet command.
	@param targetClass The class current packet is being generated for. Null if unavailable.
	@param targetName The target's name current packet is being generated for. Empty if unavailable.
	@param targetInstance Pointer to the instance current packet is being generated for. Null if unavailable.
	@param[out] out_packetCommand Generated packet command. Results are appended.
	*/
	virtual void generate(
		const SdlOutputClauses& clauses,
		const SdlOutputContext& ctx,
		std::string_view targetName,
		const ISdlResource* targetInstance,
		std::string& out_packetCommand) const = 0;
};

}// end namespace ph
