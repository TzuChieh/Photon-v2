#pragma once

#include "DataIO/FileSystem/IResourceIdentifierResolver.h"

#include <string>
#include <string_view>

namespace ph
{

class Path;
class SdlIOContext;
class ResourceIdentifier;

/*! @brief Type of the SDL Resource Identifier (SRI).
*/
enum class ESriType
{
	Unknown = 0,
	Bundle,
	External
};

class SdlResourceLocator : public IResourceIdentifierResolver
{
public:
	/*! @brief Check whether the string is a SDL resource identifier.
	Checks the format of the string only. Does not test whether the identifier actually points to 
	a valid resource or not. This utility is useful when a quick test for the content of the value
	string is desired.
	*/
	static bool isRecognized(std::string_view sdlValueStr);

public:
	explicit SdlResourceLocator(const SdlIOContext& ctx);

	bool resolve(ResourceIdentifier& identifier) override;

	Path toPath(std::string_view sri) const;
	std::string toBundleSRI(std::string_view sri) const;
	std::string toExternalSRI(std::string_view sri) const;

	/*! @brief Get an identifier representing the path.
	@return A bundle identifier resolving to @p path.
	*/
	ResourceIdentifier toBundleIdentifier(const Path& path) const;

	/*! @brief Get an identifier representing the path.
	@return An external identifier resolving to @p path.
	*/
	ResourceIdentifier toExternalIdentifier(const Path& path) const;

	/*! @brief Tries to get type information from a potential SRI.
	@param identifier The identifier to get type from. Does not need to be a SRI.
	@return Type of the identifier. `ESriType::Unknown` if not a SRI.
	*/
	ESriType getType(std::string_view identifier) const;

	// TODO: method to migrate external to bundle or vice versa

private:
	static auto determineType(std::string_view sri) -> ESriType;

	const SdlIOContext& m_ctx;
};

}// end namespace ph
