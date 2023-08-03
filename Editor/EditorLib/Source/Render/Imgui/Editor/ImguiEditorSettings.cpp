#include "Render/Imgui/Editor/ImguiEditorSettings.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"

#include "ThirdParty/DearImGui.h"

#include <Common/logging.h>

namespace ph::editor
{

ImguiEditorSettings::ImguiEditorSettings()
	: m_category(ECategory::Startup)
{}

void ImguiEditorSettings::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* isOpening)
{
	// Center this window on the first time it is used
	ImGuiCond windowLayoutCond = ImGuiCond_FirstUseEver;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(
		viewport->GetCenter(),
		windowLayoutCond,
		ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(
		{viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.8f},
		windowLayoutCond);

	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();

	// Left child: settings category selection
	ImGui::BeginChild(
		"selection",
		ImVec2(
			std::max(ImGui::GetFontSize() * 10, ImGui::GetContentRegionAvail().x * 0.2f),
			0),
		true);
	buildCategorySelectionContent();
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: settings for the category
	ImGui::BeginChild(
		"settings",
		ImVec2(0, 0),
		true);
	buildSettingsContent();
	ImGui::EndChild();

	ImGui::End();
}

void ImguiEditorSettings::buildCategorySelectionContent()
{
	if(ImGui::BeginListBox("##category_listbox", ImVec2(-FLT_MIN, -FLT_MIN)))
	{
		for(int i = 0; i < IM_ARRAYSIZE(CATEGORY_NAMES); ++i)
		{
			const bool isSelected = (m_category == i);
			if(ImGui::Selectable(CATEGORY_NAMES[i], isSelected))
			{
				m_category = i;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}
}

void ImguiEditorSettings::buildSettingsContent()
{

}

}// end namespace ph::editor
