#include "SDL/sdl_parser.h"
#include "Utility/string_utils.h"
#include "SDL/sdl_exceptions.h"

#include <Common/assertion.h>

namespace ph::sdl_parser
{

std::string_view get_reference(std::string_view referenceToken)
{
	// Remove any leading and trailing blank characters
	auto token = string_utils::trim(referenceToken);

	// Should at least contain a specifier + non-empty reference (name)
	if(token.size() < 2)
	{
		throw_formatted<SdlLoadError>(
			"syntax error: reference is empty or comes without any specifier (<{}> was given)",
			referenceToken);
	}

	const char specifier = token.front();

	// Remove the specifier
	token.remove_prefix(1);
	token = string_utils::trim_head(token);

	switch(specifier)
	{
	// Token contains a name (optionally with quotes)
	case '@':
	{
		if(token.empty())
		{
			throw SdlLoadError(
				"reference requires a name");
		}

		// Token is a name with quotes
		if(token.front() == '"')
		{
			// Should at least contain 3 characters: e.g., `"x"`
			if(token.size() >= 3 && token.back() == '"')
			{
				// Remove the double quotes
				return token.substr(1, token.size() - 2);
			}
			else
			{
				throw_formatted<SdlLoadError>(
					"syntax error: resource name missing ending double quote and/or the name is empty, "
					"<{}> was given",
					token);
			}
		}
		// Token is the name
		else
		{
			// TODO: could contain whitespaces, this can be checked and reported
			return token;
		}
	}
		break;

	default:
		throw_formatted<SdlLoadError>(
			"syntax error: unknown reference specifier <{}>",
			specifier);
	}
}

std::string_view trim_double_quotes(std::string_view str)
{
	// First remove any enclosing whitespaces
	str = string_utils::trim(str);

	if(str.size() >= 2 && str.starts_with('"') && str.ends_with('"'))
	{
		str.remove_prefix(1);
		str.remove_suffix(1);
	}

	return str;
}

bool is_double_quoted(std::string_view str)
{
	// First remove any enclosing whitespaces
	str = string_utils::trim(str);

	return str.size() >= 2 && str.starts_with('"') && str.ends_with('"');
}

}// end namespace ph::sdl_parser
