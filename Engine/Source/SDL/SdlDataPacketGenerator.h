#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Utility/TSpan.h"

#include <string_view>
#include <cstddef>

namespace ph
{

class SdlClass;
class SdlOutputClauses;
class ISdlResource;

/*! @brief Generating a standalone collection of SDL clauses.
*/
class SdlDataPacketGenerator
{
public:
	SdlDataPacketGenerator();
	explicit SdlDataPacketGenerator(Path sceneWorkingDirectory);
	virtual ~SdlDataPacketGenerator();

	// TODO: parameters like binary form? multi-thread?

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(Path directory);

protected:
	/*!
	@return Whether to generate command for this class.
	*/
	virtual bool beginPacket(const SdlClass* targetClass) = 0;

	virtual void packetGenerated(std::string_view packetStr) = 0;
	virtual void binaryPacketGenerated(TSpanView<std::byte> packetBytes) = 0;
	virtual void endPacket() = 0;

	void generate(const ISdlResource& resource);

private:
	Path m_sceneWorkingDirectory;
};

}// end namespace ph
