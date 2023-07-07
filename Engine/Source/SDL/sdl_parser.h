#pragma once

#include <string>
#include <string_view>

namespace ph::sdl_parser
{

/*! @brief Retrieve reference from a single reference token.
A reference token is not the same as a reference. Reference token contains additional specifiers
such as a reference indicator (the @ character). For example, `@name` is a reference token, while
`name` is a reference (with `@` being its specifier).
*/
std::string get_reference(std::string_view referenceToken);

}// end namespace ph::sdl_parser