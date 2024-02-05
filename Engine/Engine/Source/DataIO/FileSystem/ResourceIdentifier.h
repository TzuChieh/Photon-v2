#pragma once

#include "DataIO/FileSystem/Path.h"

#include <Common/Utility/string_utils.h>

#include <string>
#include <variant>

namespace ph
{

class IResourceIdentifierResolver;

/*! @brief An general identifier that points to some resource.
This is the most general form of a resource identifier, typically refers to as 
Photon Resource Identifier (PRI) thoughout the engine.
*/
class ResourceIdentifier final
{
public:
	/*! @brief Creates empty identifier that refers to nothing.
	*/
	ResourceIdentifier();

	/*! @brief Creates identifier from a character sequence.
	*/
	explicit ResourceIdentifier(std::string identifier);

	/*! @brief Work out the resource the identifier points to.
	*/
	bool resolve(IResourceIdentifierResolver& resolver);
	
	/*! @brief Whether the target resource is identified.
	*/
	bool isResolved() const;

	/*! @brief Whether there is an identifier.
	An identifier may not exist in the following situations:
	1. There was no identifier specified. 
	2. A resolved target was set directly (see set methods for more details).
	*/
	bool hasIdentifier() const;

	const std::string& getIdentifier() const;
	Path getPath() const;
	std::string getString() const;

	/*! @brief Directly set a path to resource.
	This will also clear any existing identifier as we cannot automatically choose one of the many
	types of identifiers that resolves to this path (many-to-one mapping).
	*/
	void setPath(Path path);

	/*! @brief Whether any information is contained.
	@return `true` if any identifier exists, be it unresolved or resolved; otherwise return `false`.
	*/
	bool isEmpty() const;

	std::string toString() const;

private:
	friend class IResourceIdentifierResolver;

	void setResolved(Path resolved);
	void setResolved(std::string resolved);
	void clearResolved();

	using ResolvedIdentifier = std::variant<
		std::monostate,
		Path,
		std::string>;

	std::string m_identifier;
	ResolvedIdentifier m_resolvedIdentifier;
};

}// end namespace ph

PH_DEFINE_INLINE_TO_STRING_FORMATTER(ph::ResourceIdentifier);
