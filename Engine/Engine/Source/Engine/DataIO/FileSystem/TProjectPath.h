#pragma once

#include "Engine/DataIO/FileSystem/Filesystem.h"
#include "Engine/EEngineProject.h"
#include "Engine/DataIO/FileSystem/Path.h"

#include <Common/assertion.h>

#include <string>
#include <utility>

namespace ph
{

enum class EProjectDirectory
{
	Config,
	InternalResource,
	Resource,
	Intermediate,
	Script
};

/*! @brief Project directory path wrapper. Implicitly convertible to `Path`.
@param identifier Currently accepts a path relative to the selected project directory in the
build/install directory.
*/
template<EEngineProject PROJ, EProjectDirectory DIR>
class TProjectPath final
{
public:
	explicit TProjectPath(std::string identifier);

	Path getPath() const;
	const std::string& getIdentifier() const;

	operator Path () const;

private:
	static Path getProjectDirectory();

	std::string m_identifier;
};

template<EEngineProject PROJ, EProjectDirectory DIR>
inline TProjectPath<PROJ, DIR>::TProjectPath(std::string identifier)
	: m_identifier(std::move(identifier))
{
	PH_ASSERT(Path(m_identifier).isRelative());
}

template<EEngineProject PROJ, EProjectDirectory DIR>
inline Path TProjectPath<PROJ, DIR>::getPath() const
{
	return getProjectDirectory() / m_identifier;
}

template<EEngineProject PROJ, EProjectDirectory DIR>
inline const std::string& TProjectPath<PROJ, DIR>::getIdentifier() const
{
	return m_identifier;
}

template<EEngineProject PROJ, EProjectDirectory DIR>
inline TProjectPath<PROJ, DIR>::operator Path () const
{
	return getPath();
}

template<EEngineProject PROJ, EProjectDirectory DIR>
inline Path TProjectPath<PROJ, DIR>::getProjectDirectory()
{
	if constexpr(DIR == EProjectDirectory::Config)
	{
		return Filesystem::getConfigDirectory() / to_string(PROJ);
	}
	else if constexpr(DIR == EProjectDirectory::InternalResource)
	{
		return Filesystem::getInternalResourceDirectory() / to_string(PROJ);
	}
	else if constexpr(DIR == EProjectDirectory::Resource)
	{
		return Filesystem::getResourceDirectory() / to_string(PROJ);
	}
	else if constexpr(DIR == EProjectDirectory::Intermediate)
	{
		return Filesystem::getIntermediateDirectory() / to_string(PROJ);
	}
	else
	{
		static_assert(DIR == EProjectDirectory::Script);
		return Filesystem::getScriptDirectory() / to_string(PROJ);
	}
}

template<EEngineProject PROJ>
using TConfigPath = TProjectPath<PROJ, EProjectDirectory::Config>;

template<EEngineProject PROJ>
using TInternalResourcePath = TProjectPath<PROJ, EProjectDirectory::InternalResource>;

template<EEngineProject PROJ>
using TResourcePath = TProjectPath<PROJ, EProjectDirectory::Resource>;

template<EEngineProject PROJ>
using TIntermediatePath = TProjectPath<PROJ, EProjectDirectory::Intermediate>;

template<EEngineProject PROJ>
using TScriptPath = TProjectPath<PROJ, EProjectDirectory::Script>;

using EngineResourcePath             = TResourcePath<EEngineProject::Engine>;
using EngineTestResourcePath         = TResourcePath<EEngineProject::EngineTest>;
using EngineIntermediatePath         = TIntermediatePath<EEngineProject::Engine>;
using EngineTestIntermediatePath     = TIntermediatePath<EEngineProject::EngineTest>;
using EngineDeepTestIntermediatePath = TIntermediatePath<EEngineProject::EngineDeepTest>;
using SDLGenInternalResourcePath     = TInternalResourcePath<EEngineProject::SDLGen>;
using EditorLibInternalResourcePath  = TInternalResourcePath<EEngineProject::EditorLib>;
using IntersectErrorIntermediatePath = TIntermediatePath<EEngineProject::IntersectError>;

}// end namespace ph
