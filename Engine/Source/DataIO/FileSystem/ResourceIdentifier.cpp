#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "DataIO/FileSystem/IResourceIdentifierResolver.h"

#include <utility>

namespace ph
{

ResourceIdentifier::ResourceIdentifier()
	: ResourceIdentifier("")
{}

ResourceIdentifier::ResourceIdentifier(std::string identifier)
	: m_identifier(std::move(identifier))
	, m_resolvedIdentifier(std::monostate{})
{}

void ResourceIdentifier::resolve(IResourceIdentifierResolver& resolver)
{
	resolver.resolve(*this);
}
	
void ResourceIdentifier::setIdentifier(std::string identifier)
{
	m_identifier = std::move(identifier);
}

void ResourceIdentifier::setResolved(Path resolved)
{
	m_resolvedIdentifier = std::move(resolved);
}

void ResourceIdentifier::setResolved(std::string resolved)
{
	m_resolvedIdentifier = std::move(resolved);
}

void ResourceIdentifier::clearResolved()
{
	m_resolvedIdentifier = std::monostate{};
}

bool ResourceIdentifier::isResolved() const
{
	return std::holds_alternative<std::monostate>(m_resolvedIdentifier);
}

const std::string& ResourceIdentifier::getIdentifier() const
{
	return m_identifier;
}

Path ResourceIdentifier::getResolvedPath() const
{
	return std::holds_alternative<Path>(m_resolvedIdentifier)
		? std::get<Path>(m_resolvedIdentifier)
		: Path{};
}

std::string ResourceIdentifier::getResolvedString() const
{
	return std::holds_alternative<std::string>(m_resolvedIdentifier)
		? std::get<std::string>(m_resolvedIdentifier)
		: std::string{};
}

}// end namespace ph
