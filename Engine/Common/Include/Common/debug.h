#pragma once

#include <string>

namespace ph
{

/*! @brief Break into the debugger programmatically.
*/
void debug_break();

/*! @brief Collect information for the call stack.
@return A string containing information for the current call stack. Tracing back from this function call.
*/
std::string obtain_stack_trace();

}// end namespace ph

/*! @brief Macro equivalent for `ph::debug_break()`.
*/
#define PH_DEBUG_BREAK() ::ph::debug_break()
