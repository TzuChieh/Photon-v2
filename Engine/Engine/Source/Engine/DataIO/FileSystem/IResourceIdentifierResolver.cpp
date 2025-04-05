#include "Engine/DataIO/FileSystem/IResourceIdentifierResolver.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/DataIO/FileSystem/ResourceIdentifier.h"

#include <utility>

namespace ph
{

void IResourceIdentifierResolver::setResolved(ResourceIdentifier& identifier, Path resolved)
{
	identifier.setResolved(std::move(resolved));
}

void IResourceIdentifierResolver::setResolved(ResourceIdentifier& identifier, std::string resolved)
{
	identifier.setResolved(std::move(resolved));
}

void IResourceIdentifierResolver::clearResolved(ResourceIdentifier& identifier)
{
	identifier.clearResolved();
}

}// end namespace ph
