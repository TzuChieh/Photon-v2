#include "Designer/IO/DesignerSceneMetaInfo.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include "ThirdParty/NLohmannJSON.h"

#include <Common/logging.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>
#include <DataIO/Stream/FormattedTextInputStream.h>
#include <DataIO/Stream/FormattedTextOutputStream.h>
#include <SDL/sdl_exceptions.h>
#include <Utility/Timestamp.h>

#include <vector>
#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(DesignerSceneMetaInfo, Designer);

namespace
{

inline Path get_meta_info_file(const Path& directory, std::string_view infoName)
{
	return directory / (std::string(infoName) + ".pdsm");
}

}// end anonymous namespace

DesignerSceneMetaInfo::DesignerSceneMetaInfo()
	: m_objectNameToObjectMetaInfo()
{}

void DesignerSceneMetaInfo::gather(const DesignerScene& scene)
{
	// Find by the common base type `DesignerObject` effectively retrieves all valid objects
	std::vector<DesignerObject*> objs;
	scene.findObjectsByType(objs);

	for(DesignerObject* obj : objs)
	{
		ObjectMetaInfo objMeta;
		objMeta.parentName = obj->getParent() ? obj->getParent()->getName() : "";
		objMeta.isTicking = obj->getState().has(EObjectState::Ticking);
		objMeta.isRenderTicking = obj->getState().has(EObjectState::RenderTicking);

		setObjectMetaInfo(obj->getName(), std::move(objMeta));
	}
}

void DesignerSceneMetaInfo::load(const Path& directory, std::string_view infoName)
{
	const Path file = get_meta_info_file(directory, infoName);

	std::string fileContent;
	try
	{
		FormattedTextInputStream stream(file);
		stream.readAll(&fileContent);
	}
	catch(const IOException& e)
	{
		throw_formatted<SdlLoadError>(
			"error while loading scene meta info {} -> {}",
			file, e.whatStr());
	}

	nlohmann::json jsonObj = nlohmann::json::parse(fileContent);

	// Read object meta info
	for(const auto& [objName, objMetaJsonObj] : jsonObj["objects"].items())
	{
		ObjectMetaInfo objMeta;
		objMeta.parentName = objMetaJsonObj["parent-name"].get<std::string>();
		objMeta.isTicking = objMetaJsonObj["is-ticking"].get<bool>();
		objMeta.isRenderTicking = objMetaJsonObj["is-render-ticking"].get<bool>();

		setObjectMetaInfo(objName, std::move(objMeta));
	}
}

void DesignerSceneMetaInfo::save(const Path& directory, std::string_view infoName) const
{
	nlohmann::json jsonObj;

	jsonObj["timestamp"] = Timestamp().toYMDHMSMilliseconds();

	// Write object meta info
	for(const auto& [objName, objMeta] : m_objectNameToObjectMetaInfo)
	{
		jsonObj["objects"][objName]["parent-name"] = objMeta.parentName;
		jsonObj["objects"][objName]["is-ticking"] = objMeta.isTicking;
		jsonObj["objects"][objName]["is-render-ticking"] = objMeta.isRenderTicking;
	}

	const Path file = get_meta_info_file(directory, infoName);

	try
	{
		Filesystem::createDirectories(directory);

		FormattedTextOutputStream stream(file);
		//stream.writeString(nlohmann::to_string(jsonObj));
		stream.writeString(jsonObj.dump(1, '\t'));
	}
	catch(const IOException& e)
	{
		throw_formatted<SdlSaveError>(
			"error while writing scene meta info {} -> {}",
			file, e.whatStr());
	}
}

auto DesignerSceneMetaInfo::getObject(std::string_view objectName) const
-> const ObjectMetaInfo*
{
	const auto& result = m_objectNameToObjectMetaInfo.find(objectName);
	if(result == m_objectNameToObjectMetaInfo.end())
	{
		return nullptr;
	}

	return &result->second;
}

void DesignerSceneMetaInfo::setObjectMetaInfo(
	std::string_view objectName,
	ObjectMetaInfo objMeta,
	const bool expectNoOverwrite)
{
	if(expectNoOverwrite &&
	   m_objectNameToObjectMetaInfo.find(objectName) != m_objectNameToObjectMetaInfo.end())
	{
		PH_LOG_WARNING(DesignerSceneMetaInfo,
			"Designer object {} meta info is being overwritten",
			objectName);
	}

	m_objectNameToObjectMetaInfo[std::string(objectName)] = std::move(objMeta);
}

}// end namespace ph::editor
