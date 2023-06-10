#include "SDL/SdlResourceLocator.h"
#include "SDL/sdl_helpers.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/string_utils.h"
#include "Common/assertion.h"
#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_exceptions.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"

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

SdlResourceLocator::SdlResourceLocator(const SdlIOContext& ctx)
	: IResourceIdentifierResolver()
	, m_ctx(ctx)
{}

void SdlResourceLocator::resolve(ResourceIdentifier& identifier)
{
	switch(getType(identifier.getIdentifier()))
	{
	case ESdlResourceIdentifier::Bundle:
	case ESdlResourceIdentifier::External:
		identifier.setResolved(toPath(identifier.getIdentifier()));
		break;

	default:
		identifier.clearResolved();
	}
}

Path SdlResourceLocator::toPath(std::string_view identifier) const
{
	switch(getType(identifier))
	{
	case ESdlResourceIdentifier::Bundle:
	{
		return bundle_identifier_to_path(identifier, m_ctx.getWorkingDirectory());
	}

	case ESdlResourceIdentifier::External:
	{
		return external_identifier_to_path(identifier);
	}

	default:
		throw_formatted<SdlException>(
			"failed to generate path: {} is not a valid SDL resource identifier",
			identifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return Path();
}

std::string SdlResourceLocator::toBundleIdentifier(std::string_view identifier) const
{
	switch(getType(identifier))
	{
	case ESdlResourceIdentifier::Bundle:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(identifier));
	}

	case ESdlResourceIdentifier::External:
	{
		std::filesystem::path resourceRelPath = std::filesystem::relative(
			external_identifier_to_path(identifier).toStdPath(),
			m_ctx.getWorkingDirectory().toStdPath());

		if(resourceRelPath.empty())
		{
			throw_formatted<SdlException>(
				"failed to generate bundle identifier for <{}> (with bundle directory {})",
				identifier, m_ctx.getWorkingDirectory());
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
			identifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return std::string();
}

std::string SdlResourceLocator::toExternalIdentifier(std::string_view identifier) const
{
	switch(getType(identifier))
	{
	case ESdlResourceIdentifier::Bundle:
	{
		const Path& path = bundle_identifier_to_path(identifier, m_ctx.getWorkingDirectory());
		return "ext:" + path.toAbsoluteString();
	}

	case ESdlResourceIdentifier::External:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(identifier));
	}

	default:
		throw_formatted<SdlException>(
			"failed to generate external identifier: {} is not a valid SDL resource identifier",
			identifier);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return std::string();
}

ESdlResourceIdentifier SdlResourceLocator::getType(std::string_view resourceIdentifier) const
{
	return determineType(resourceIdentifier);
}

auto SdlResourceLocator::determineType(std::string_view identifier)
-> ESdlResourceIdentifier
{
	// Remove leading blank characters
	const auto headTrimmedStr = string_utils::trim_head(identifier);
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
