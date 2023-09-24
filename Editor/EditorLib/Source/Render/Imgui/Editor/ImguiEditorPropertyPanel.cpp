#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include "ThirdParty/DearImGui.h"

#include <utility>

namespace ph::editor
{

ImguiEditorPropertyPanel::ImguiEditorPropertyPanel(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_layout()
{}

void ImguiEditorPropertyPanel::buildWindow(const char* windowIdName, bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_AlwaysVerticalScrollbar;// so width can be fixed--no scrollbar popping

	if(!ImGui::Begin(windowIdName, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	// TODO: disable this button if object is flat
	const SdlClass* selectedClass = nullptr;
	getEditorUI().getObjectTypeMenu().buildMenuButton(
		PH_IMGUI_PLUS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Child Object ", 
		selectedClass);

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	ImGui::End();
}

auto ImguiEditorPropertyPanel::getAttributes() const
-> Attributes
{
	return {
		.title = "Properties",
		.icon = PH_IMGUI_PROPERTIES_ICON,
		.tooltip = "Properties",
		.preferredDockingLot = EImguiPanelDockingLot::LowerRight,
		.isOpenedByDefault = true,
		.isCloseable = false};
}

void ImguiEditorPropertyPanel::setLayout(UIPropertyLayout layout)
{
	m_layout = std::move(layout);
}

void ImguiEditorPropertyPanel::clearLayout()
{
	m_layout.clear();
}

}// end namespace ph::editor
