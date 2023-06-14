#pragma once

#include "SDL/RawResourceCollection.h"
#include "SDL/ISdlReferenceGroup.h"
#include "DataIO/FileSystem/Path.h"

namespace ph
{

/*! @brief Data that describe a scene.
Represents the result of running SDL commands.
*/
class SceneDescription : public ISdlReferenceGroup
{
public:
	SceneDescription();

	std::shared_ptr<ISdlResource> get(std::string_view resourceName) const override;
	bool has(std::string_view resourceName) const override;

	RawResourceCollection& getResources();
	const RawResourceCollection& getResources() const;

	RawResourceCollection& getPhantoms();
	const RawResourceCollection& getPhantoms() const;
	
	const Path& getWorkingDirectory() const;
	void setWorkingDirectory(Path directory);

private:
	Path m_workingDirectory;
	RawResourceCollection m_resources;
	RawResourceCollection m_phantomResources;
};

inline RawResourceCollection& SceneDescription::getResources()
{
	return m_resources;
}

inline const RawResourceCollection& SceneDescription::getResources() const
{
	return m_resources;
}

inline RawResourceCollection& SceneDescription::getPhantoms()
{
	return m_phantomResources;
}

inline const RawResourceCollection& SceneDescription::getPhantoms() const
{
	return m_phantomResources;
}

inline const Path& SceneDescription::getWorkingDirectory() const
{
	return m_workingDirectory;
}

}// end namespace ph
