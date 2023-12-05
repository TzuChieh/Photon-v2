#include "Utility/SemanticVersion.h"

#include <Utility/string_utils.h>

#include <format>

namespace ph
{

SemanticVersion::SemanticVersion(std::string_view version) :
	SemanticVersion()
{
	using namespace string_utils;

	version = trim(version);

	if(auto dotPos = version.find('.'); dotPos != std::string_view::npos)
	{
		m_major = parse_int<uint16>(version.substr(0, dotPos));
		version = version.substr(dotPos + 1);
	}

	if(auto dotPos = version.find('.'); dotPos != std::string_view::npos)
	{
		m_minor = parse_int<uint16>(version.substr(0, dotPos));
		version = version.substr(dotPos + 1);
	}

	// If there is anything left in the string, it should be patch number
	if(!version.empty())
	{
		m_patch = parse_int<uint16>(version);
	}
}

std::string SemanticVersion::toString() const
{
	return std::format("{}.{}.{}", m_major, m_minor, m_patch);
}

}// end namespace ph
