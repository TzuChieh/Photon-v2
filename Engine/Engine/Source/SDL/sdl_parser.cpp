#include "SDL/sdl_parser.h"
#include "SDL/sdl_exceptions.h"

#include <Common/assertion.h>
#include <Utility/string_utils.h>

namespace ph::sdl_parser
{

bool is_single_name_with_specifier(std::string_view valueToken)
{
	if(!starts_with_specifier(valueToken))
	{
		return false;
	}

	// The token must contain at least one specifier + name
	valueToken = string_utils::trim_head(valueToken);
	PH_ASSERT(is_specifier(valueToken[0]));

	// Remove the first specifier
	valueToken.remove_prefix(1);
	valueToken = string_utils::trim_head(valueToken);
	
	// Remove the first name (quoted)
	if(!valueToken.empty() && valueToken.starts_with('"'))
	{
		valueToken.remove_prefix(1);
		while(!valueToken.empty())
		{
			if(valueToken.starts_with('"'))
			{
				valueToken.remove_prefix(1);
				break;
			}
			else if(valueToken.starts_with("\\\""))
			{
				valueToken.remove_prefix(2);
			}
			else
			{
				valueToken.remove_prefix(1);
			}
		}
	}
	// Remove the first name (not quoted)
	else
	{
		while(!valueToken.empty() && !string_utils::is_whitespace(valueToken[0]))
		{
			valueToken.remove_prefix(1);
		}
	}

	// If the token contains more non-whitespace content, it does not contain only single
	// specifier + name. Additional content could be more specifier + names, or arbitrary values.
	valueToken = string_utils::trim_head(valueToken);
	return valueToken.empty();
}

auto get_name_with_specifier(std::string_view nameToken)
-> std::pair<std::string_view, char>
{
	// Remove any leading and trailing blank characters
	auto token = string_utils::trim(nameToken);

	// Should at least contain a specifier + non-empty reference (name)
	if(token.size() < 2)
	{
		throw_formatted<SdlLoadError>(
			"syntax error: token is empty or comes without any specifier (<{}> was given)",
			nameToken);
	}

	const char specifier = token.front();

	// Remove the specifier
	token.remove_prefix(1);
	token = string_utils::trim_head(token);

	switch(specifier)
	{
	// Token contains a name (optionally with quotes)
	case persistent_specifier:
	case cached_specifier:
	{
		if(token.empty())
		{
			throw SdlLoadError(
				"missing name");
		}

		// Token is a name with quotes
		if(token.front() == '"')
		{
			// Should at least contain 3 characters: e.g., `"x"`
			if(token.size() >= 3 && token.back() == '"')
			{
				// Remove the double quotes
				return {token.substr(1, token.size() - 2), specifier};
			}
			else
			{
				throw_formatted<SdlLoadError>(
					"syntax error: name missing ending double quote and/or the name is empty, "
					"<{}> was given", token);
			}
		}
		// Token is the name
		else
		{
			// TODO: could contain whitespaces, this can be checked and reported
			return {token, specifier};
		}
	}
		break;

	default:
		throw_formatted<SdlLoadError>(
			"syntax error: unknown specifier <{}>", specifier);
	}
}

std::string_view get_reference(std::string_view referenceToken)
{
	auto [name, specifier] = get_name_with_specifier(referenceToken);
	if(specifier != persistent_specifier)
	{
		throw_formatted<SdlLoadError>(
			"non-persistent reference is not supported (reference: <{}>)", name);
	}

	return name;
}

std::string_view get_data_packet_name(std::string_view dataPacketNameToken)
{
	auto [name, specifier] = get_name_with_specifier(dataPacketNameToken);
	if(specifier != cached_specifier)
	{
		throw_formatted<SdlLoadError>(
			"non-cached data packet is not supported (packet: <{}>)", name);
	}

	return name;
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
