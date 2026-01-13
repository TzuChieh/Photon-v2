#pragma once

#include "Engine/ph_core.h"
#include "Engine/EEngineProject.h"
#include "Engine/DataIO/FileSystem/Path.h"

#include <string>
#include <utility>

namespace ph
{

/*! @brief Project resource path wrapper. Implicitly convertible to `Path`.
*/
template<EEngineProject PROJ>
class TResourcePath final
{
public:
	explicit TResourcePath(std::string identifier);

	Path getPath() const;

	operator Path () const;

private:
	std::string m_identifier;
};

template<EEngineProject PROJ>
inline TResourcePath<PROJ>::TResourcePath(std::string identifier)
	: m_identifier(std::move(identifier))
{}

template<EEngineProject PROJ>
inline Path TResourcePath<PROJ>::getPath() const
{
	return get_resource_directory(PROJ) / m_identifier;
}

template<EEngineProject PROJ>
inline TResourcePath<PROJ>::operator Path () const
{
	return getPath();
}

using EngineResource     = TResourcePath<EEngineProject::Engine>;
using EngineTestResource = TResourcePath<EEngineProject::EngineTest>;

}// end namespace ph
