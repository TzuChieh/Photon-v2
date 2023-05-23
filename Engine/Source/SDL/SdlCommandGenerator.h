#pragma once

#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlOutputClauses.h"

#include <string_view>
#include <string>

namespace ph
{

class SdlClass;
class ISdlResource;
class SdlOutputClauses;
class SdlOutputContext;
class SceneDescription;
class SdlDependencyResolver;

class SdlCommandGenerator
{
public:
	SdlCommandGenerator();
	explicit SdlCommandGenerator(Path sceneWorkingDirectory);
	virtual ~SdlCommandGenerator();

	// TODO: parameters like binary form? multi-thread?

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(Path directory);

protected:
	/*!
	@return Whether to generate command for this class.
	*/
	virtual bool beginCommand(const SdlClass* targetClass) = 0;

	/*! @brief Save target resource into output clauses.
	*/
	virtual void saveResource(
		const ISdlResource* resource,
		const SdlClass* resourceClass,
		SdlOutputClauses& clauses,
		const SdlDependencyResolver* resolver) = 0;

	virtual void commandGenerated(std::string_view commandStr) = 0;
	virtual void endCommand() = 0;

	void generateScene(const SceneDescription& scene);

private:
	struct OutputBuffer
	{
		std::string commandStr;
		SdlOutputClauses clauses;

		void clear();
	};

	static void generateLoadCommand(
		const ISdlResource& resource, 
		const SdlClass* resourceClass,
		const std::string& resourceName,
		const SdlOutputClauses& clauses,
		std::string& out_commandStr);

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
