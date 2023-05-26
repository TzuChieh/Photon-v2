#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class SdlOutputClauses;

/*! @brief Generating a standalone collection of SDL clauses.
*/
class SdlDataPacketGenerator
{
public:
	explicit SdlDataPacketGenerator(Path sceneWorkingDirectory);
	virtual ~SdlDataPacketGenerator();

	/*! @brief Turn all data in the @p clauses into a packet.
	*/
	virtual void generate(
		const SdlOutputClauses& clauses,
		std::string& out_packetCommand) const = 0;

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(Path directory);

private:
	Path m_sceneWorkingDirectory;
};

}// end namespace ph
