#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>
#include <variant>

namespace ph
{

class IResourceIdentifierResolver;

class ResourceIdentifier final
{
public:
	/*! @brief Creates empty identifier that refers to nothing.
	*/
	ResourceIdentifier();

	/*! @brief Creates identifier from a character sequence.
	*/
	explicit ResourceIdentifier(std::string identifier);

	void resolve(IResourceIdentifierResolver& resolver);
	
	void setIdentifier(std::string identifier);
	void setResolved(Path resolved);
	void setResolved(std::string resolved);
	void clearResolved();

	bool isResolved() const;
	const std::string& getIdentifier() const;
	Path getResolvedPath() const;
	std::string getResolvedString() const;

private:
	using ResolvedIdentifier = std::variant<
		std::monostate,
		Path,
		std::string>;

	std::string m_identifier;
	ResolvedIdentifier m_resolvedIdentifier;
};

}// end namespace ph
