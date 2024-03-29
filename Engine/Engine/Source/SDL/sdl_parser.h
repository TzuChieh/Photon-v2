#pragma once

#include <string_view>
#include <utility>

namespace ph::sdl_parser
{

inline constexpr char persistent_specifier = '@';
inline constexpr char cached_specifier = '$';

bool is_specifier(char ch);
bool starts_with_specifier(std::string_view valueToken);
bool is_single_name_with_specifier(std::string_view valueToken);

/*! @brief Retrieve name and specifier from a single name token.
*/
auto get_name_with_specifier(std::string_view nameToken)
-> std::pair<std::string_view, char>;

/*! @brief Retrieve reference from a single reference token.
A reference token is not the same as a reference. Reference token contains additional specifiers
such as a persistence indicator (the @ character). For example, `@name` is a reference token, while
`name` is a reference (with `@` being its specifier), and this function will return `name`
given `@name`.
*/
std::string_view get_reference(std::string_view referenceToken);

std::string_view get_data_packet_name(std::string_view dataPacketNameToken);

/*! @brief Remove the enclosing double quotes and whitespaces from `str`.
Note that if only a single double quote exists (not in a pair), it does not count as being enclosed
by double quotes and thus will not be removed.
@return The substring with enclosing double quotes and whitespaces removed. If `str` is not
enclosed by double quotes, a trimmed `str` is returned.
*/
std::string_view trim_double_quotes(std::string_view str);

bool is_double_quoted(std::string_view str);

}// end namespace ph::sdl_parser

#include "SDL/sdl_parser.ipp"
