#pragma once

#include "SDL/SdlCommandParser.h"
#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class SceneDescription;
class FormattedTextInputStream;

/*! @brief Loading utility for renderer scene.
Loads .p2 (Photon-v2) file.
*/
class SdlSceneFileReader : public SdlCommandParser
{
public:
	SdlSceneFileReader();
	SdlSceneFileReader(std::string sceneName, const Path& sceneWorkingDirectory);
	~SdlSceneFileReader() override;

	/*!
	@param[in, out] scene The target for parsed results. Effectively set current scene to @p scene 
	then start reading. If @p scene was null, read to the current scene instead.
	*/
	void read(SceneDescription* scene = nullptr);

	void setSceneName(std::string sceneName);

	/*!
	@param[in, out] scene The target for parsed results. @p scene will be the current scene.
	*/
	void setScene(SceneDescription* scene);

protected:
	bool beginCommand(ESdlCommandType commandType, const SdlClass* targetClass) override;

	ISdlResource* createResource(
		std::string_view resourceName,
		const SdlClass* resourceClass,
		ESdlCommandType commandType) override;

	void initResource(
		ISdlResource* resource,
		const SdlClass* resourceClass,
		std::string_view resourceName,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) override;

	ISdlResource* getResource(std::string_view resourceName, ESdlTypeCategory category) override;

	void runExecutor(
		std::string_view executorName,
		const SdlClass* targetClass,
		ISdlResource* targetResource,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) override;

	void commandVersionSet(const SemanticVersion& version) override;
	void endCommand() override;

private:
	std::string m_sceneName;
	FormattedTextInputStream* m_fileStream;
	SceneDescription* m_scene;
};

}// end namespace ph