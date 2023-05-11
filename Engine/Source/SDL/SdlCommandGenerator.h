#pragma once

#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlOutputClauses.h"

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
	@return Whether to generate command for this class.
	*/
	virtual bool beginCommand(const SdlClass* targetClass) = 0;

	virtual void commandGenerated(std::string_view commandStr) = 0;
	virtual void endCommand() = 0;

	void generateScene(const SceneDescription& scene);

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(Path directory);

private:
	struct OutputBuffer
	{
		std::string commandStr;
		SdlOutputClauses clauses;

		void clear();
	};

	static void generateLoadCommand(
		const SdlOutputContext& ctx,
		const ISdlResource& resource, 
		const std::string& resourceName,
		OutputBuffer& out_result);

	static void appendFullSdlType(
		const SdlClass* clazz,
		std::string& out_commandStr);

	static void appendClause(
		const SdlOutputClause& clause,
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
	clauses.clear();
}

}// end namespace ph
