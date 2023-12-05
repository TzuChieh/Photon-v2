#include "SDL/SdlResourceLocator.h"
#include "SDL/sdl_helpers.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/FileSystem/Filesystem.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/sdl_exceptions.h"

#include <Common/assertion.h>
#include <Utility/string_utils.h>

#include <utility>

namespace ph
{

namespace
{

inline Path bundle_SRI_to_path(std::string_view bundleIdentifier, const Path& bundleRoot)
{
	auto identifier = string_utils::trim_head(bundleIdentifier);
	PH_ASSERT_GE(identifier.size(), 1);

	// Remove the ":" prefix on the front, rest is path relative to the bundle root
	identifier.remove_prefix(1);
	return bundleRoot / identifier;
}

inline Path external_SRI_to_path(std::string_view externalIdentifier)
{
	auto identifier = string_utils::trim_head(externalIdentifier);
	PH_ASSERT_GE(identifier.size(), 4);

	// Remove the "ext:" prefix on the front, rest is absolute path
	identifier.remove_prefix(4);
	return Path(identifier);
}

inline std::string path_to_external_SRI(const Path& path)
{
	// Already a path--it must point directly to the target
	return "ext:" + path.toString();
}

}// end anonymous namespace

bool SdlResourceLocator::isRecognized(std::string_view sdlValueStr)
{
	return SdlResourceLocator::determineType(sdlValueStr) != ESriType::Unknown;
}

SdlResourceLocator::SdlResourceLocator(const SdlIOContext& ctx)
	: IResourceIdentifierResolver()
	, m_ctx(ctx)
{}

bool SdlResourceLocator::resolve(ResourceIdentifier& identifier)
{
	switch(getType(identifier.getIdentifier()))
	{
	case ESriType::Bundle:
	case ESriType::External:
		setResolved(identifier, toPath(identifier.getIdentifier()));
		return true;

	default:
		clearResolved(identifier);
		return false;
	}
}

Path SdlResourceLocator::toPath(std::string_view sri) const
{
	switch(getType(sri))
	{
	case ESriType::Bundle:
	{
		return bundle_SRI_to_path(sri, m_ctx.getWorkingDirectory());
	}

	case ESriType::External:
	{
		return external_SRI_to_path(sri);
	}
	}

	throw_formatted<SdlException>(
		"failed to generate path: {} is not a valid SDL resource identifier",
		sri);
}

std::string SdlResourceLocator::toBundleSRI(std::string_view sri) const
{
	switch(getType(sri))
	{
	case ESriType::Bundle:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(sri));
	}

	case ESriType::External:
	{
		Path resourceRelPath = Filesystem::makeRelative(
			external_SRI_to_path(sri), 
			m_ctx.getWorkingDirectory());

		if(resourceRelPath.isEmpty())
		{
			throw_formatted<SdlException>(
				"failed to generate bundle identifier for <{}> (with bundle directory {})",
				sri, m_ctx.getWorkingDirectory());
		}

		// Note that `resourceRelPath` may contain parent folder director (e.g., "../aaa"). 
		// Making sure the resource is within bundle directory is ideal, but not yet done 
		// (can be quite involved--consider the case with multiple directors: "../a/b/./../c", 
		// or even with various OS links). 
		// Some easy-to-check cases can be implemented in the future though.

		return ":" + resourceRelPath.toString();
	}
	}

	throw_formatted<SdlException>(
		"failed to generate bundle identifier: {} is not a valid SDL resource identifier",
		sri);
}

std::string SdlResourceLocator::toExternalSRI(std::string_view sri) const
{
	switch(getType(sri))
	{
	case ESriType::Bundle:
	{
		const Path& path = bundle_SRI_to_path(sri, m_ctx.getWorkingDirectory());
		return "ext:" + path.toAbsoluteString();
	}

	case ESriType::External:
	{
		// The identifier is already what we want
		return std::string(string_utils::trim_head(sri));
	}
	}

	throw_formatted<SdlException>(
		"failed to generate external identifier: {} is not a valid SDL resource identifier",
		sri);
}

ResourceIdentifier SdlResourceLocator::toBundleIdentifier(const Path& path) const
{
	ResourceIdentifier identifier(toBundleSRI(path_to_external_SRI(path)));
	setResolved(identifier, path);
	return identifier;
}

ResourceIdentifier SdlResourceLocator::toExternalIdentifier(const Path& path) const
{
	ResourceIdentifier identifier(path_to_external_SRI(path));
	setResolved(identifier, path);
	return identifier;
}

ESriType SdlResourceLocator::getType(std::string_view identifier) const
{
	return determineType(identifier);
}

auto SdlResourceLocator::determineType(std::string_view identifier)
-> ESriType
{
	// Remove leading blank characters
	const auto headTrimmedStr = string_utils::trim_head(identifier);
	if(headTrimmedStr.empty())
	{
		return ESriType::Unknown;
	}

	PH_ASSERT_GE(headTrimmedStr.size(), 1);
	switch(headTrimmedStr[0])
	{
	case ':':
		// Valid SDL bundled resource identifier starts with a single colon ":"
		return ESriType::Bundle;

	case 'e':
		// Valid SDL external resource identifier starts with "ext:"
		if(headTrimmedStr.size() >= 4 &&
		   headTrimmedStr[1] == 'x' &&
		   headTrimmedStr[2] == 't' &&
		   headTrimmedStr[3] == ':')
		{
			return ESriType::External;
		}
		else
		{
			return ESriType::Unknown;
		}
	}

	return ESriType::Unknown;
}

}// end namespace ph
