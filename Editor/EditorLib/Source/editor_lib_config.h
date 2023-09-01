#pragma once

#include <cstddef>

namespace ph::editor::config
{

inline constexpr bool USE_BUFFERED_RENDER_THREAD = true;
inline constexpr std::size_t NUM_RENDER_THREAD_BUFFERED_FRAMES = 2;

inline constexpr bool USE_BUFFERED_GHI_THREAD = false;
inline constexpr std::size_t NUM_GHI_THREAD_BUFFERED_FRAMES = 2;

}// end namespace ph::editor::config
