#pragma once

#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlOutputPayloads.h"

#include <string_view>
#include <string>

namespace ph
{

class SdlClass;
class ISdlResource;
class SdlOutputContext;
class SceneDescription;

class SdlCommandGenerator
{
public:
	SdlCommandGenerator();
	explicit SdlCommandGenerator(Path sceneWorkingDirectory);
	virtual ~SdlCommandGenerator();

	// TODO: parameters like binary form? multi-thread?

	/*!
	@return Whether to keep generating command for this class.
	*/
	virtual bool beginCommand(const SdlClass* targetClass) = 0;

	virtual void generatedCommand(std::string_view commandStr) = 0;

	/*!
	@return Whether to continue the whole command generating process.
	*/
	virtual bool endCommand() = 0;

	void generateScene(const SceneDescription& scene);

	const Path& getSceneWorkingDirectory() const;

private:
	struct OutputBuffer
	{
		std::string commandStr;
		SdlOutputPayloads payloads;

		void clear();
	};

	static void generateLoadCommand(
		const SdlOutputContext& ctx,
		const ISdlResource& resource, 
		const std::string& resourceName,
		OutputBuffer& out_result);

	static void appendFullSdlType(
		const SdlClass& clazz,
		std::string& out_commandStr);

	static void appendClause(
		const SdlOutputPayload& payload,
		std::string& out_commandStr);

private:
	Path m_sceneWorkingDirectory;
};

inline const Path& SdlCommandGenerator::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

inline void SdlCommandGenerator::OutputBuffer::clear()
{
	commandStr.clear();
	payloads.clear();
}

}// end namespace ph
