#pragma once

#include "SDL/SdlCommandGenerator.h"
#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class SceneDescription;
class FormattedTextFileOutputStream;

class SdlSceneFileWriter : public SdlCommandGenerator
{
public:
	SdlSceneFileWriter();
	SdlSceneFileWriter(std::string sceneName, Path sceneWorkingDirectory);
	~SdlSceneFileWriter() override;

	bool beginCommand(const SdlClass* targetClass) override;
	void generatedCommand(std::string_view commandStr) override;
	bool endCommand() override;

	void save(const SceneDescription& scene);

private:
	std::string m_sceneName;
	FormattedTextFileOutputStream* m_fileStream;
};

}// end namespace ph
