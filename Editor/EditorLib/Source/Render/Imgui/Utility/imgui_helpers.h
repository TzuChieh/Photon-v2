#pragma once

#include <Utility/TSpan.h>

#include <string_view>

namespace ph::editor::imgui
{

void text_unformatted(std::string_view text);

/*! @brief Copy string to a buffer. The result is always null-terminated.
@param dstBuffer The buffer to copy into. Its size should never be 0. 
*/
void copy_to(TSpan<char> dstBuffer, std::string_view srcStr);

}// end ph::editor::imgui
