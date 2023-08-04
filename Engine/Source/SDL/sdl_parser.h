#pragma once

#include <string_view>

namespace ph::sdl_parser
{

/*! @brief Retrieve reference from a single reference token.
A reference token is not the same as a reference. Reference token contains additional specifiers
such as a reference indicator (the @ character). For example, `@name` is a reference token, while
`name` is a reference (with `@` being its specifier).
*/
std::string_view get_reference(std::string_view referenceToken);

/*! @brief Remove the enclosing double quotes and whitespaces from `str`.
Note that if only a single double quote exists (not in a pair), it does not count as being enclosed
by double quotes and thus will not be removed.
@return The substring with enclosing double quotes and whitespaces removed. If `str` is not
enclosed by double quotes, a trimmed `str` is returned.
*/
std::string_view trim_double_quotes(std::string_view str);

bool is_double_quoted(std::string_view str);

}// end namespace ph::sdl_parser
