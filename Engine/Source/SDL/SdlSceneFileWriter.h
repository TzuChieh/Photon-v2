#pragma once

#include "SDL/SdlCommandGenerator.h"
#include "SDL/SdlDependencyResolver.h"

#include <string>

namespace ph
{

class SceneDescription;
class FormattedTextOutputStream;

/*! @brief Saving utility for renderer scene.
Writes .p2 (Photon-v2) file.
*/
class SdlSceneFileWriter : public SdlCommandGenerator
{
public:
	SdlSceneFileWriter();

	/*! @brief Creates reader for all engine classes.
	*/
	SdlSceneFileWriter(std::string sceneName, const Path& sceneWorkingDirectory);

	/*! @brief Creates reader for a custom set of classes.
	*/
	SdlSceneFileWriter(
		TSpanView<const SdlClass*> targetClasses,
		std::string sceneName, 
		const Path& sceneWorkingDirectory);

	~SdlSceneFileWriter() override;

	/*! @brief Writes the scene to the writer-specified destination.
	*/
	void write(const SceneDescription& scene);

	void setSceneName(std::string sceneName);

protected:
	bool beginCommand(
		const SdlClass* targetClass,
		SdlOutputContext* out_ctx) override;

	void endCommand() override;

	void saveResource(
		const ISdlResource* resource,
		const SdlOutputContext& ctx,
		SdlOutputClauses& clauses) override;

	void commandGenerated(
		std::string_view commandStr,
		const SdlOutputContext& ctx) override;

private:
	void saveSceneToFile(const SceneDescription& scene, const Path& filePath);

	std::string m_sceneName;
	SdlDependencyResolver m_resolver;
	FormattedTextOutputStream* m_fileStream;
};

}// end namespace ph
