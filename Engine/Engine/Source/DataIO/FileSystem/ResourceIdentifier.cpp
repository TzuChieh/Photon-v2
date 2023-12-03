#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "DataIO/FileSystem/IResourceIdentifierResolver.h"

#include <utility>
#include <format>

namespace ph
{

ResourceIdentifier::ResourceIdentifier()
	: ResourceIdentifier("")
{}

ResourceIdentifier::ResourceIdentifier(std::string identifier)
	: m_identifier(std::move(identifier))
	, m_resolvedIdentifier(std::monostate{})
{}

bool ResourceIdentifier::resolve(IResourceIdentifierResolver& resolver)
{
	return resolver.resolve(*this);
}

void ResourceIdentifier::setPath(Path path)
{
	m_identifier.clear();
	m_resolvedIdentifier = std::move(path);
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
	return !std::holds_alternative<std::monostate>(m_resolvedIdentifier);
}

bool ResourceIdentifier::hasIdentifier() const
{
	return !m_identifier.empty();
}

const std::string& ResourceIdentifier::getIdentifier() const
{
	return m_identifier;
}

Path ResourceIdentifier::getPath() const
{
	return std::holds_alternative<Path>(m_resolvedIdentifier)
		? std::get<Path>(m_resolvedIdentifier)
		: Path{};
}

std::string ResourceIdentifier::getString() const
{
	return std::holds_alternative<std::string>(m_resolvedIdentifier)
		? std::get<std::string>(m_resolvedIdentifier)
		: std::string{};
}

bool ResourceIdentifier::isEmpty() const
{
	return !hasIdentifier() && !isResolved();
}

std::string ResourceIdentifier::toString() const
{
	return std::format(
		"identifier: {}, resolved={}",
		getIdentifier(), isResolved());
}

}// end namespace ph
