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

	/*!
	The reader takes a pointer to scene and cache it since a possible use case is to treat the reader
	as a command interpreter. Not specifying a scene on each method call makes a cleaner interface.
	@param[in, out] scene The target for parsed results. Effectively set current scene to @p scene
	then start reading. If @p scene was null, read to the current scene instead.
	*/
	void read(DesignerScene* scene = nullptr);

	/*!
	@param[in, out] scene The target for parsed results. @p scene will be the current scene.
	*/
	void setScene(DesignerScene* scene);

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

	DesignerScene* m_scene;
	DesignerSceneMetaInfo m_metaInfo;
	string_utils::TStdUnorderedStringMap<DesignerObject*> m_nameToNewObjs;
};

}// end namespace ph
