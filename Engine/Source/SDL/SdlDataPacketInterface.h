#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

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
	*/
	virtual void parse(
		std::string_view packetCommand,
		SdlInputClauses& out_clauses) const = 0;

	/*! @brief Turn all data in the @p clauses into a packet command.
	*/
	virtual void generate(
		const SdlOutputClauses& clauses,
		std::string& out_packetCommand) const = 0;

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(const Path& directory);

private:
	Path m_sceneWorkingDirectory;
};

}// end namespace ph
