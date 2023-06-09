#pragma once

#include <string>
#include <string_view>

namespace ph
{

class Path;
class SdlIOContext;

enum class ESdlResourceIdentifier
{
	Unknown = 0,
	Bundle,
	External
};

class SdlResourceLocator final
{
public:
	/*! @brief Check whether the string is a SDL resource identifier.
	Checks the format of the string only. Does not test whether the identifier actually points to 
	a valid resource or not. This utility is useful when a quick test for the content of the value
	string is desired.
	*/
	static bool isRecognized(std::string_view sdlValueStr);

public:
	explicit SdlResourceLocator(std::string_view resourceIdentifier);
	explicit SdlResourceLocator(std::string resourceIdentifier);

	Path toPath(const SdlIOContext& ctx) const;
	std::string toBundleIdentifier(const SdlIOContext& ctx) const;
	std::string toExternalIdentifier(const SdlIOContext& ctx) const;
	ESdlResourceIdentifier getType() const;

	// TODO: method to migrate external to bundle or vice versa

private:
	static auto determineType(std::string_view resourceIdentifier) -> ESdlResourceIdentifier;

	std::string m_resourceIdentifier;
	ESdlResourceIdentifier m_type;
};

inline ESdlResourceIdentifier SdlResourceLocator::getType() const
{
	return m_type;
}

}// end namespace ph
