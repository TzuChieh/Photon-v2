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

SdlResourceLocator::SdlResourceLocator(std::string_view resourceIdentifier)
	: SdlResourceLocator(std::string(resourceIdentifier))
{}

SdlResourceLocator::SdlResourceLocator(std::string resourceIdentifier)
	: m_resourceIdentifier(std::move(resourceIdentifier))
{}

Path SdlResourceLocator::toPath(const SdlIOContext& ctx) const
{
	if(sdl::is_bundled_resource_identifier(m_resourceIdentifier))
	{
		auto identifier = string_utils::trim_head(m_resourceIdentifier);
		PH_ASSERT_GE(identifier.size(), 1);

		// Remove the colon on the front, rest is path relative to the bundle root
		identifier.remove_prefix(1);
		return ctx.getWorkingDirectory() / identifier;
	}
	// If not a bundle identifier, then it is a oridinary path (no matter relative or absolute)
	else
	{
		return Path(m_resourceIdentifier);
	}
}

std::string SdlResourceLocator::toBundleIdentifier(const SdlIOContext& ctx) const
{
	if(sdl::is_bundled_resource_identifier(m_resourceIdentifier))
	{
		return std::string(string_utils::trim_head(m_resourceIdentifier));
	}
	// If not a bundle identifier, then it is a oridinary path (no matter relative or absolute)
	else
	{
		std::filesystem::path resourceRelPath = std::filesystem::relative(
			m_resourceIdentifier, 
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

		return "//" + resourceRelPath.string();
	}
}

}// end namespace ph
