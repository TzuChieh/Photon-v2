#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include "ThirdParty/DearImGui.h"

#include <Common/logging.h>

#include <string>

namespace ph::editor
{

namespace
{

constexpr const char* OPEN_SCENE_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open Scene";
constexpr const char* SAVE_SCENE_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save Scene";

}// end anonymous namespace

namespace
{

inline void save_scene(
	Editor& editor, 
	const ImguiFileSystemDialog& fsDialog,
	const bool shouldSaveWithFolder)
{
	auto dirPath = fsDialog.getSelectedDirectory();
	if(dirPath.isEmpty())
	{
		PH_DEFAULT_LOG(Warning,
			"cannot save scene: no directory specified");
		return;
	}

	PH_ASSERT(!dirPath.isEmpty());
	//editor.saveScene(dirPath);
}

}// end anonymous namespace

ImguiEditorSceneManager::ImguiEditorSceneManager(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)
	
	, m_selectedSceneIdx(static_cast<std::size_t>(-1))
{}

void ImguiEditorSceneManager::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	{
		if(ImGui::Button("Make Selection Active"))
		{
			if(m_selectedSceneIdx < editor.numScenes())
			{
				editor.setActiveScene(m_selectedSceneIdx);
			}
		}

		ImGui::SameLine();

		DesignerScene* activeScene = editor.getActiveScene();
		ImGui::Text("Active Scene: %s%s",
			activeScene ? activeScene->getName().c_str() : "(none)",
			activeScene ? activeScene->isPaused() ? " (paused)" : " (running)" : "");
	}
	
	// List box for all opened scenes
	// Custom size: use all width, 5 items tall
	if(ImGui::BeginListBox("##scenes_listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for(std::size_t sceneIdx = 0; sceneIdx < editor.numScenes(); ++sceneIdx)
		{
			// Scene name may not be unique
			ImGui::PushID(static_cast<int>(sceneIdx));

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

			ImGui::PopID();
		}
		ImGui::EndListBox();
	}

	{
		ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();
		if(ImGui::Button("Open"))
		{
			fsDialog.openPopup(OPEN_SCENE_TITLE);
		}

		fsDialog.buildFileSystemDialogPopupModal(
			OPEN_SCENE_TITLE, 
			getEditorUI());

		if(fsDialog.dialogClosed())
		{
			auto items = fsDialog.getSelectedItems();
			for(const auto& item : items)
			{
				PH_DEFAULT_LOG(Note, "selected: {}", item.toString());
			}
		}

		// TODO
	}

	{
		ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();
		if(ImGui::Button("Save"))
		{
			fsDialog.openPopup(SAVE_SCENE_TITLE);
		}

		fsDialog.buildFileSystemDialogPopupModal(
			SAVE_SCENE_TITLE, 
			getEditorUI(),
			{.canSelectItem = false, .canSelectDirectory = true, .requiresDirectorySelection = true});

		if(fsDialog.dialogClosed())
		{
			//save_scene(editor, fsDialog, m_shouldSaveWithFolder);
		}
	}

	ImGui::End();
}

auto ImguiEditorSceneManager::getAttributes() const
-> Attributes
{
	return {
		.title = "Scene Manager",
		.icon = PH_IMGUI_SCENE_MANAGER_ICON,
		.tooltip = "Scene Manager",
		.preferredDockingLot = EImguiPanelDockingLot::UpperRight,
		.useSidebar = true};
}

}// end namespace ph::editor
