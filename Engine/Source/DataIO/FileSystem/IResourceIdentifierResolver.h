#pragma once

#include <string>

namespace ph
{

class ResourceIdentifier;
class Path;

class IResourceIdentifierResolver
{
public:
	IResourceIdentifierResolver() = default;
	IResourceIdentifierResolver(const IResourceIdentifierResolver& other) = default;
	IResourceIdentifierResolver(IResourceIdentifierResolver&& other) noexcept = default;
	virtual ~IResourceIdentifierResolver() = default;

	/*! @brief Resolve the @p identifier;
	@return Whether the resolve was successful.
	*/
	virtual bool resolve(ResourceIdentifier& identifier) = 0;

	IResourceIdentifierResolver& operator = (const IResourceIdentifierResolver& rhs) = default;
	IResourceIdentifierResolver& operator = (IResourceIdentifierResolver&& rhs) noexcept = default;

protected:
	static void setResolved(ResourceIdentifier& identifier, Path resolved);
	static void setResolved(ResourceIdentifier& identifier, std::string resolved);
	static void clearResolved(ResourceIdentifier& identifier);
};

}// end namespace ph
