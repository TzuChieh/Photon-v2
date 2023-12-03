#pragma once

#include "SDL/SdlCommandParser.h"
#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class SceneDescription;

/*! @brief Loading utility for renderer scene.
Loads .p2 (Photon-v2) file.
*/
class SdlSceneFileReader : public SdlCommandParser
{
public:
	SdlSceneFileReader();

	/*! @brief Creates reader for all engine classes.
	*/
	SdlSceneFileReader(const Path& sceneFile, const Path& sceneWorkingDirectory);

	/*! @brief Creates reader for a custom set of classes.
	*/
	SdlSceneFileReader(
		TSpanView<const SdlClass*> targetClasses, 
		const Path& sceneFile,
		const Path& sceneWorkingDirectory);

	~SdlSceneFileReader() override;

	/*!
	@param[in, out] scene The target for parsed results. Effectively set current scene to @p scene 
	then start reading. If @p scene was null, read to the current scene instead.
	*/
	void read(SceneDescription* scene = nullptr);

	void setSceneFile(Path sceneFile);

	/*!
	The reader takes a pointer to scene and cache it since a possible use case is to treat the reader
	as a command interpreter. Not specifying a scene on each method call makes a cleaner interface.
	@param[in, out] scene The target for parsed results. @p scene will be the current scene.
	*/
	void setScene(SceneDescription* scene);

protected:
	bool beginCommand(
		ESdlCommandType commandType, 
		const SdlClass* targetClass,
		SdlInputContext* out_ctx) override;

	void endCommand() override;

	ISdlResource* createResource(
		std::string_view resourceName,
		const SdlInputContext& ctx,
		ESdlCommandType commandType) override;

	void initResource(
		ISdlResource* resource,
		const SdlInputContext& ctx,
		std::string_view resourceName,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) override;

	ISdlResource* getResource(
		std::string_view resourceName, 
		const SdlInputContext& ctx) override;

	void runExecutor(
		std::string_view executorName,
		const SdlInputContext& ctx,
		ISdlResource* targetResource,
		SdlInputClauses& clauses,
		ESdlCommandType commandType) override;

	void commandVersionSet(
		const SemanticVersion& version,
		const SdlInputContext& ctx) override;

private:
	Path m_sceneFile;
	SceneDescription* m_scene;
};

}// end namespace ph
