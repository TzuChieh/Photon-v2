#pragma once

#include "SDL/RawResourceCollection.h"

namespace ph
{

/*! @brief Data that describe a scene.
Represents the result of running SDL commands.
*/
class SceneDescription final
{
public:
	SceneDescription();

	RawResourceCollection& getResources();
	const RawResourceCollection& getResources() const;

	RawResourceCollection& getPhantoms();
	const RawResourceCollection& getPhantoms() const;
	
private:
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

}// end namespace ph