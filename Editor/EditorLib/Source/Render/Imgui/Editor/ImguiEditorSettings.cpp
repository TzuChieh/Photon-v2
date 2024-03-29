#include "Render/Imgui/Editor/ImguiEditorSettings.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "App/Misc/EditorSettings.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"

#include "ThirdParty/DearImGui.h"

#include <Common/logging.h>

namespace ph::editor
{

ImguiEditorSettings::ImguiEditorSettings(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)
	
	, m_category(ECategory::General)
	, m_stringDisplayBuffer(512, '\0')
{}

void ImguiEditorSettings::buildWindow(const char* windowIdName, bool* isOpening)
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

	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

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

	// TODO: use SDL description as tool tip

	// Right child: settings for the category
	ImGui::BeginChild(
		"settings",
		ImVec2(0, 0),
		true);
	buildSettingsContent();
	ImGui::EndChild();

	ImGui::End();
}

auto ImguiEditorSettings::getAttributes() const
-> Attributes
{
	return {
		.title = "Editor Settings",
		.icon = PH_IMGUI_SETTINGS_ICON,
		.tooltip = "Editor Settings",
		.useSidebar = true};
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
	switch(m_category)
	{
	case ECategory::General:
		buildGeneralCategoryContent();
		break;

	case ECategory::Startup:
		buildStartupCategoryContent();
		break;

	case ECategory::Control:
		buildControlCategoryContent();
		break;
	}
}

void ImguiEditorSettings::buildGeneralCategoryContent()
{
	EditorSettings& settings = getEditorUI().getEditor().getSettings();

	ImGui::Checkbox("Development Mode", &settings.isDevelopmentMode);
}

void ImguiEditorSettings::buildStartupCategoryContent()
{
	EditorSettings& settings = getEditorUI().getEditor().getSettings();

	// Setting default scene
	{
		ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();
		if(ImGui::Button("Browse"))
		{
			fsDialog.openPopup("Select Default Scene");
		}

		fsDialog.buildFileSystemDialogPopupModal(
			"Select Default Scene",
			getEditorUI());

		if(fsDialog.dialogClosed())
		{
			if(fsDialog.hasSelectedItem())
			{
				settings.defaultSceneFile = fsDialog.getSelectedTarget().toAbsolute();
			}
		}

		ImGui::SameLine();
		ImGui::InputText(
			"Default Scene", 
			m_stringDisplayBuffer.data(), 
			settings.defaultSceneFile.toNativeString(m_stringDisplayBuffer),
			ImGuiInputTextFlags_ReadOnly);
	}

	ImGui::Checkbox("Load Default Scene On Start-up", &settings.loadDefaultSceneOnStartup);
}

void ImguiEditorSettings::buildControlCategoryContent()
{
	EditorSettings& settings = getEditorUI().getEditor().getSettings();

	ImGui::SliderFloat("Image Zoom Sensitivity", &settings.imageZoomSensitivity, 0.1f, 10.0f);
}

}// end namespace ph::editor
