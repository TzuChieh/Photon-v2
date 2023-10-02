#pragma once

#include "Designer/IO/DesignerSceneMetaInfo.h"

#include <SDL/SdlCommandParser.h>
#include <Utility/string_utils.h>

#include <string>
#include <unordered_map>

namespace ph::editor
{

class DesignerScene;
class DesignerObject;

/*! @brief Loading utility for designer scene.
Loads .pds (Photon Designer Scene) file.
*/
class DesignerSceneReader : public SdlCommandParser
{
public:
	DesignerSceneReader();
	explicit DesignerSceneReader(const Path& sceneWorkingDirectory);
	~DesignerSceneReader() override;

	/*! @brief Read the entire designer scene.
	Scene information must be provided via `setSceneInfo()` prior to calling this method.
	*/
	void read();

	/*!
	The reader takes a name and pointer to scene and cache them since a possible use case is to treat
	the reader as a command interpreter. Not specifying additional attributes on each method call makes
	a cleaner interface.
	@param sceneName Name of the scene. Note that while `DesignerScene` contains scene name, this
	parameter is still required as some scene information must be acquired with scene name and
	`scene` may not have this information yet (or stale, since the reading process has not started).
	@param[in, out] scene The target for parsed results. @p scene will be the current scene.
	*/
	void setSceneInfo(std::string sceneName, DesignerScene* scene);

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
	void readScene();
	void readSceneMetaInfo();

	std::string m_sceneName;
	DesignerScene* m_scene;
	DesignerSceneMetaInfo m_metaInfo;
	string_utils::TStdUnorderedStringMap<DesignerObject*> m_nameToNewObjs;
};

}// end namespace ph
