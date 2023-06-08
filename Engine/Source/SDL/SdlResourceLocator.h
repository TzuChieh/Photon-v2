#pragma once

#include <string>
#include <string_view>

namespace ph
{

class Path;
class SdlIOContext;

class SdlResourceLocator final
{
public:
	explicit SdlResourceLocator(std::string_view resourceIdentifier);
	explicit SdlResourceLocator(std::string resourceIdentifier);

	Path toPath(const SdlIOContext& ctx) const;
	std::string toBundleIdentifier(const SdlIOContext& ctx) const;

private:
	std::string m_resourceIdentifier;
};

}// end namespace ph
