#pragma once

#include "SDL/SdlCommandGenerator.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlDependencyResolver.h"

#include <string>

namespace ph
{

class SceneDescription;
class FormattedTextOutputStream;

class SdlSceneFileWriter : public SdlCommandGenerator
{
public:
	SdlSceneFileWriter();
	SdlSceneFileWriter(std::string sceneName, Path sceneWorkingDirectory);
	~SdlSceneFileWriter() override;

	void write(const SceneDescription& scene);

	void setSceneName(std::string sceneName);

protected:
	bool beginCommand(const SdlClass* targetClass) override;

	void saveResource(
		const ISdlResource* resource,
		const SdlClass* resourceClass,
		SdlOutputClauses& clauses) override;

	void commandGenerated(std::string_view commandStr) override;
	void endCommand() override;

private:
	std::string m_sceneName;
	SdlDependencyResolver m_resolver;
	FormattedTextOutputStream* m_fileStream;
};

}// end namespace ph
