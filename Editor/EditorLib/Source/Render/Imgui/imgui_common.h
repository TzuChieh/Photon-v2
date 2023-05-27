#pragma once

#include <Common/logging.h>

namespace ph { class Path; }

namespace ph::editor
{

PH_DECLARE_LOG_GROUP(DearImGui);

void imgui_show_demo_window(bool* isOpening = nullptr);

Path get_imgui_data_directory();

}// end namespace ph::editor
