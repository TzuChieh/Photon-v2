#pragma once

namespace ph
{

class ResourceIdentifier;

class IResourceIdentifierResolver
{
public:
	IResourceIdentifierResolver() = default;
	IResourceIdentifierResolver(const IResourceIdentifierResolver& other) = default;
	IResourceIdentifierResolver(IResourceIdentifierResolver&& other) noexcept = default;
	virtual ~IResourceIdentifierResolver() = default;

	virtual void resolve(ResourceIdentifier& identifier) = 0;

	IResourceIdentifierResolver& operator = (const IResourceIdentifierResolver& rhs) = default;
	IResourceIdentifierResolver& operator = (IResourceIdentifierResolver&& rhs) noexcept = default;
};

}// end namespace ph
