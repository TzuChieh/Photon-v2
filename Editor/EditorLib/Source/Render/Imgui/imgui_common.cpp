#include "Render/Imgui/imgui_common.h"
#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(DearImGui, Module);

void show_imgui_demo_window(bool* const isOpening)
{
	ImGui::ShowDemoWindow(isOpening);
}

}// end namespace ph::editor
