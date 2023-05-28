#include "Render/Imgui/imgui_common.h"
#include "ThirdParty/DearImGui.h"
#include "ph_editor.h"

#include <DataIO/FileSystem/Path.h>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(DearImGui, Module);

void imgui_show_demo_window(bool* const isOpening)
{
	ImGui::ShowDemoWindow(isOpening);
}

Path get_imgui_data_directory()
{
	return get_editor_data_directory() / "ImGui";
}

}// end namespace ph::editor
