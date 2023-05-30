#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>
#include <string_view>

namespace ph
{

class SdlResourceIdentifier final
{
public:
	SdlResourceIdentifier(
		std::string_view identifier, 
		const Path&      workingDirectory);

	const Path& getPathToResource() const;

	// TODO: ctor should take some asset context object instead

private:
	Path m_pathToResource;
};

// In-header Implementations:

inline SdlResourceIdentifier::SdlResourceIdentifier(
	const std::string_view identifier,
	const Path&            workingDirectory) :
	
	m_pathToResource(workingDirectory.append(Path(std::string(identifier))))
{}

inline const Path& SdlResourceIdentifier::getPathToResource() const
{
	return m_pathToResource;
}

}// end namespace ph
