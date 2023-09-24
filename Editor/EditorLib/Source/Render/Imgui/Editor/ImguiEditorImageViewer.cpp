#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorImageViewer::ImguiEditorImageViewer(ImguiEditorUIProxy editorUI)
	: ImguiEditorPanel(editorUI)
{}

ImguiEditorImageViewer::~ImguiEditorImageViewer()
{}

void ImguiEditorImageViewer::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	

	ImGui::End();
}

auto ImguiEditorImageViewer::getAttributes() const
-> Attributes
{
	return {
		.title = "Image Viewer",
		.icon = PH_IMGUI_IMAGE_ICON,
		.preferredDockingLot = EImguiPanelDockingLot::Center};
}

}// end namespace ph::editor
