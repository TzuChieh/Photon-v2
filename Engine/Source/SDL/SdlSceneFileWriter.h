#pragma once

#include "SDL/SdlCommandGenerator.h"
#include "DataIO/FileSystem/Path.h"

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

	bool beginCommand(const SdlClass* targetClass) override;
	void commandGenerated(std::string_view commandStr) override;
	void endCommand() override;

	void write(const SceneDescription& scene);

	void setSceneName(std::string sceneName);

private:
	std::string m_sceneName;
	FormattedTextOutputStream* m_fileStream;
};

}// end namespace ph
