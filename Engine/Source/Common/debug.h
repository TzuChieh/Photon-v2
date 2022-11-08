#pragma once

namespace ph
{

void debug_break();

}// end namespace ph

#define PH_DEBUG_BREAK() ::ph::debug_break()
