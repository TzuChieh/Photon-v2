#pragma once

#include <string>

namespace ph
{

void debug_break();
std::string obtain_stack_trace();

}// end namespace ph

#define PH_DEBUG_BREAK() ::ph::debug_break()
