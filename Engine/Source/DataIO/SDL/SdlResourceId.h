#pragma once

#include <cstdint>

namespace ph
{

using SdlResourceId = std::uint64_t;

inline constexpr SdlResourceId EMPTY_SDL_RESOURCE_ID = 0;

/*!
@note Thread-safe.
*/
SdlResourceId gen_sdl_resource_id();

}// end namespace ph
