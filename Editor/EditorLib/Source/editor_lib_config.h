#pragma once

#include <cstddef>

namespace ph::editor::config
{

inline constexpr bool ENABLE_RENDER_SYSTEM_ASYNC_FILE_READING = true;

inline constexpr std::size_t NUM_RENDER_THREAD_BUFFERED_FRAMES = 2;

}// end namespace ph::editor::config
