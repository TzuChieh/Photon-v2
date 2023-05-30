#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>
#include <string_view>

namespace ph
{

class SdlClass;
class SdlInputClauses;
class SdlOutputClauses;

/*! @brief Parsing & generating a group of SDL clauses.
*/
class SdlDataPacketInterface
{
public:
	explicit SdlDataPacketInterface(const Path& sceneWorkingDirectory);
	virtual ~SdlDataPacketInterface();

	/*! @brief Turn a packet command into @p out_clauses.
	@param targetClass The class current packet is being parsed for. Null if there is no target.
	@param targetName The target's name current packet is being parsed for. Empty if there is no target.
	@param[out] out_clauses Parsed clauses. Results are appended.
	*/
	virtual void parse(
		std::string_view packetCommand,
		const SdlClass* targetClass,
		std::string_view targetName,
		SdlInputClauses& out_clauses) const = 0;

	/*! @brief Turn all data in the @p clauses into a packet command.
	@param targetClass The class current packet is being generated for. Null if there is no target.
	@param targetName The target's name current packet is being generated for. Empty if there is no target.
	@param[out] out_packetCommand Generated packet command. Results are appended.
	*/
	virtual void generate(
		const SdlOutputClauses& clauses,
		const SdlClass* targetClass,
		std::string_view targetName,
		std::string& out_packetCommand) const = 0;

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(const Path& directory);

private:
	Path m_sceneWorkingDirectory;
};

}// end namespace ph
