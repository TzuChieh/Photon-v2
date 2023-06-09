#include "SDL/SdlResourceLocator.h"
#include "SDL/sdl_helpers.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/string_utils.h"
#include "Common/assertion.h"
#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_exceptions.h"

#include <utility>
#include <filesystem>

namespace ph
{

namespace
{

inline Path bundle_identifier_to_path(std::string_view bundleIdentifier, const Path& bundleRoot)
{
	auto identifier = string_utils::trim_head(bundleIdentifier);
	PH_ASSERT_GE(identifier.size(), 1);

	// Remove the ":" prefix on the front, rest is path relative to the bundle root
	identifier.remove_prefix(1);
	return bundleRoot / identifier;
}

inline Path external_identifier_to_path(std::string_view externalIdentifier)
{
	auto identifier = string_utils::trim_head(externalIdentifier);
	PH_ASSERT_GE(identifier.size(), 4);

	// Remove the "ext:" prefix on the front, rest is absolute path
	identifier.remove_prefix(4);
	return Path(identifier);
}

}// end anonymous namespace

bool SdlResourceLocator::isRecognized(std::string_view sdlValueStr)
{
	return SdlResourceLocator::determineType(sdlValueStr) != ESdlResourceIdentifier::Unknown;
}

SdlResourceLocator::SdlResourceLocator(std::string_view resourceIdentifier)
	: SdlResourceLocator(std::string(resourceIdentifier))
{}

SdlResourceLocator::SdlResourceLocator(std::string resourceIdentifier)
	: m_resourceIdentifier(std::move(resourceIdentifier))
	, m_type(ESdlResourceIdentifier::Unknown)
{
	m_type = determineType(m_resourceIdentifier);
}

Path SdlResourceLocator::toPath(const SdlIOContext& ctx) const
{
	switch(m_type)
	{
	case ESdlResourceIdentifier::Bundle:
	{
		return bundle_identifier_to_path(m_resourceIdentifier, ctx.getWorkingDirectory());
	}

	case ESdlResourceIdentifier::External:
	{
		return external_identifier_to_path(m_resourceIdentifier);
	}

	default:
		throw_formatted<SdlException>(
			"failed to generate path: {} is not a valid SDL resource identifier",
			m_resourceIdentifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return Path();
}

std::string SdlResourceLocator::toBundleIdentifier(const SdlIOContext& ctx) const
{
	switch(m_type)
	{
	case ESdlResourceIdentifier::Bundle:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(m_resourceIdentifier));
	}

	case ESdlResourceIdentifier::External:
	{
		std::filesystem::path resourceRelPath = std::filesystem::relative(
			external_identifier_to_path(m_resourceIdentifier).toStdPath(),
			ctx.getWorkingDirectory().toStdPath());

		if(resourceRelPath.empty())
		{
			throw_formatted<SdlException>(
				"failed to generate bundle identifier for <{}> (with bundle directory {})",
				m_resourceIdentifier, ctx.getWorkingDirectory());
		}

		// Note that `resourceRelPath` may contain parent folder director (e.g., "../aaa"). 
		// Making sure the resource is within bundle directory is ideal, but not yet done 
		// (can be quite involved--consider the case with multiple directors: "../a/b/./../c", 
		// or even with various OS links). 
		// Some easy-to-check cases can be implemented in the future though.

		return ":" + resourceRelPath.string();
	}

	default:
		throw_formatted<SdlException>(
			"failed to generate bundle identifier: {} is not a valid SDL resource identifier",
			m_resourceIdentifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return std::string();
}

std::string SdlResourceLocator::toExternalIdentifier(const SdlIOContext& ctx) const
{
	switch(m_type)
	{
	case ESdlResourceIdentifier::Bundle:
	{
		const Path& path = bundle_identifier_to_path(m_resourceIdentifier, ctx.getWorkingDirectory());
		return "ext:" + path.toAbsoluteString();
	}

	case ESdlResourceIdentifier::External:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(m_resourceIdentifier));
	}

	default:
		throw_formatted<SdlException>(
			"failed to generate external identifier: {} is not a valid SDL resource identifier",
			m_resourceIdentifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return std::string();
}

auto SdlResourceLocator::determineType(std::string_view resourceIdentifier)
-> ESdlResourceIdentifier
{
	// Remove leading blank characters
	const auto headTrimmedStr = string_utils::trim_head(resourceIdentifier);
	if(headTrimmedStr.empty())
	{
		return ESdlResourceIdentifier::Unknown;
	}

	PH_ASSERT_GE(headTrimmedStr.size(), 1);
	switch(headTrimmedStr[0])
	{
	case ':':
		// Valid SDL bundled resource identifier starts with a single colon ":"
		return ESdlResourceIdentifier::Bundle;

	case 'e':
		// Valid SDL external resource identifier starts with "ext:"
		if(headTrimmedStr.size() >= 4 &&
		   headTrimmedStr[1] == 'x' &&
		   headTrimmedStr[2] == 't' &&
		   headTrimmedStr[3] == ':')
		{
			return ESdlResourceIdentifier::External;
		}
		else
		{
			return ESdlResourceIdentifier::Unknown;
		}
	}

	return ESdlResourceIdentifier::Unknown;
}

}// end namespace ph
