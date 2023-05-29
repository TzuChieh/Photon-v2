#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

#include "ThirdParty/DearImGui.h"

#include <Common/logging.h>

#include <string>

#define PH_IMGUI_OPEN_FILE_ICON ICON_MD_FOLDER_OPEN
#define PH_IMGUI_SAVE_FILE_ICON ICON_MD_SAVE

namespace ph::editor
{

constexpr const char* OPEN_SCENE_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open Scene";
constexpr const char* SAVE_SCENE_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save Scene";

namespace
{

inline ImVec2 get_file_dialog_size(const Editor& editor)
{
	return ImVec2(
		editor.dimensionHints.fileDialogPreferredWidth,
		editor.dimensionHints.fileDialogPreferredHeight);
}

}// end anonymous namespace

ImguiEditorSceneManager::ImguiEditorSceneManager()
	: m_newSceneNameBuffer()
	, m_selectedSceneIdx(static_cast<std::size_t>(-1))
{
	m_newSceneNameBuffer.fill('\0');
}

void ImguiEditorSceneManager::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();

	{
		ImguiFileSystemDialog& fsDialog = editorUI.getGeneralFileSystemDialog();
		if(ImGui::Button("Open"))
		{
			fsDialog.openPopup(OPEN_SCENE_TITLE);
		}

		fsDialog.buildFileSystemDialogPopupModal(OPEN_SCENE_TITLE, get_file_dialog_size(editor));
		if(fsDialog.selectionConfirmed())
		{
			auto items = fsDialog.getSelectedItems();
			for(const auto& item : items)
			{
				PH_DEFAULT_LOG("selected: {}", item.toString());
			}
		}
	}

	ImGui::SameLine();
	
	if(ImGui::Button("Save"))
	{
		// TODO
	}

	ImGui::SameLine();

	if(ImGui::Button("Make Active"))
	{
		if(m_selectedSceneIdx < editor.numScenes())
		{
			editor.setActiveScene(m_selectedSceneIdx);
		}
	}

	ImGui::Text("Active Scene: %s", 
		editor.getActiveScene() ? editor.getActiveScene()->getName().c_str() : "(none)");
	
	// List box for all opened scenes
	// Custom size: use all width, 5 items tall
	if(ImGui::BeginListBox("##scenes_listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for(std::size_t sceneIdx = 0; sceneIdx < editor.numScenes(); ++sceneIdx)
		{
			const bool isSelected = (sceneIdx == m_selectedSceneIdx);
			if(ImGui::Selectable(editor.getScene(sceneIdx)->getName().c_str(), isSelected))
			{
				m_selectedSceneIdx = sceneIdx;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if(ImGui::Button("New"))
	{
		editor.createScene(m_newSceneNameBuffer.data());
	}
	ImGui::SameLine();
	ImGui::InputText(
		"Scene Name",
		m_newSceneNameBuffer.data(),
		m_newSceneNameBuffer.size());

	ImGui::End();
}

}// end namespace ph::editor
