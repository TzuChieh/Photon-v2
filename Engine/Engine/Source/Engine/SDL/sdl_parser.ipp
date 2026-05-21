#pragma once

#include "Engine/SDL/sdl_parser.h"
#include "Engine/SDL/sdl_exceptions.h"

#include <Common/Utility/string_utils.h>

namespace ph::sdl_parser
{

inline constexpr char specifier_to_char(const ESpecifier specifier)
{
	switch(specifier)
	{
	case ESpecifier::None:       return '\0';
	case ESpecifier::Persistent: return persistent_specifier;
	case ESpecifier::Cached:     return cached_specifier;
	default:                     return '\0';
	}
}

inline bool is_specifier(const char ch)
{
	return ch == persistent_specifier || ch == cached_specifier;
}

inline bool starts_with_specifier(std::string_view valueToken)
{
	valueToken = string_utils::trim_head(valueToken);
	return !valueToken.empty() && is_specifier(valueToken[0]);
} 

inline std::string_view trim_specifier(
	std::string_view valueStr,
	ESpecifier* const out_trimmedSpecifier)
{
	if(out_trimmedSpecifier)
	{
		*out_trimmedSpecifier = ESpecifier::None;
	}

	const auto trimmedHead = string_utils::trim_head(valueStr);
	if(trimmedHead.empty())
	{
		return trimmedHead;
	}

	if(!is_specifier(trimmedHead.front()))
	{
		return trimmedHead;
	}

	if(out_trimmedSpecifier)
	{
		*out_trimmedSpecifier = trimmedHead.front() == persistent_specifier
			? ESpecifier::Persistent
			: ESpecifier::Cached;
	}

	auto remaining = trimmedHead;
	remaining.remove_prefix(1);
	return remaining;
}

inline std::string_view trim_name(std::string_view valueStr, std::string_view* const out_name)
{
	const auto trimmedHead = string_utils::trim_head(valueStr);
	if(trimmedHead.empty())
	{
		return trimmedHead;
	}

	auto remaining = trimmedHead;
	std::string_view parsedName;
	if(remaining.front() == '"')
	{
		remaining.remove_prefix(1);

		std::size_t parsedNameLength = 0;
		bool hasClosingQuote = false;
		auto nameTail = remaining;
		while(!nameTail.empty())
		{
			if(nameTail.starts_with('"'))
			{
				hasClosingQuote = true;
				break;
			}
			else if(nameTail.starts_with("\\\""))
			{
				nameTail.remove_prefix(2);
				parsedNameLength += 2;
			}
			else
			{
				nameTail.remove_prefix(1);
				++parsedNameLength;
			}
		}

		if(!hasClosingQuote)
		{
			throw_formatted<SdlLoadError>(
				"syntax error: missing closing quote for name <{}>", remaining);
		}

		parsedName = remaining.substr(0, parsedNameLength);
		remaining.remove_prefix(parsedNameLength + 1);
	}
	else
	{
		std::size_t nameEndPos = 0;
		while(nameEndPos < remaining.size() && !string_utils::is_whitespace(remaining[nameEndPos]))
		{
			++nameEndPos;
		}

		if(nameEndPos == 0)
		{
			return trimmedHead;
		}

		parsedName = remaining.substr(0, nameEndPos);
		remaining.remove_prefix(nameEndPos);
	}

	if(out_name)
	{
		*out_name = parsedName;
	}

	return string_utils::trim_head(remaining);
}

inline std::string_view trim_name(
	std::string_view valueStr,
	const ESpecifier expectedSpecifier,
	std::string_view* const out_name)
{
	const auto trimmedHead = string_utils::trim_head(valueStr);
	if(trimmedHead.empty())
	{
		return trimmedHead;
	}

	ESpecifier actualSpecifier = ESpecifier::None;
	const auto afterSpecifier = trim_specifier(trimmedHead, &actualSpecifier);
	if(actualSpecifier == ESpecifier::None)
	{
		return trimmedHead;
	}

	if(actualSpecifier != expectedSpecifier)
	{
		return trimmedHead;
	}

	std::string_view name;
	const auto afterName = trim_name(afterSpecifier, &name);
	if(name.empty())
	{
		throw_formatted<SdlLoadError>(
			"syntax error: missing name after specifier <{}>", specifier_to_char(expectedSpecifier));
	}

	if(out_name)
	{
		*out_name = name;
	}

	return afterName;
}

}// end namespace ph::sdl_parser
