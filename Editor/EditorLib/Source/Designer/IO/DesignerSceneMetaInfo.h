#pragma once

#include <Container/TStdUnorderedStringMap.h>

#include <string>
#include <string_view>

namespace ph { class Path; }

namespace ph::editor
{

class DesignerScene;

class DesignerSceneMetaInfo final
{
public:
	struct ObjectMetaInfo
	{
		std::string parentName = "";
		bool isTicking = false;
		bool isRenderTicking = false;

		bool isRoot() const;
	};

	DesignerSceneMetaInfo();

	/*! @brief Gather meta information from `scene`.
	*/
	void gather(const DesignerScene& scene);

	/*! @brief Load meta information from disk.
	Reads meta information file (.pdsm, Photon Designer Scene Meta-info).
	*/
	void load(const Path& directory, std::string_view infoName);

	/*! @brief Save meta information to disk.
	Writes meta information file (.pdsm, Photon Designer Scene Meta-info).
	*/
	void save(const Path& directory, std::string_view infoName) const;

	auto getObject(std::string_view objectName) const
	-> const ObjectMetaInfo*;

private:
	using StringMapType = TStdUnorderedStringMap<ObjectMetaInfo>;

	void setObjectMetaInfo(
		std::string_view objectName, 
		ObjectMetaInfo objMeta,
		bool expectNoOverwrite = true);

	StringMapType m_objectNameToObjectMetaInfo;

	// TODO: main camera
};

inline bool DesignerSceneMetaInfo::ObjectMetaInfo::isRoot() const
{
	return parentName.empty();
}

}// end namespace ph::editor
