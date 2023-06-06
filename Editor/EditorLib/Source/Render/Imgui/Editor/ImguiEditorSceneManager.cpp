#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
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

inline void save_scene(
	Editor& editor, 
	const ImguiFileSystemDialog& fsDialog,
	const bool shouldSaveWithFolder)
{
	auto optDirPath = fsDialog.getSelectedDirectory();
	if(!optDirPath)
	{
		PH_DEFAULT_LOG_WARNING(
			"cannot save scene: no directory specified");
		return;
	}
	
	auto optItemPath = fsDialog.getSelectedItem();
	if(!optItemPath)
	{
		DesignerScene* scene = editor.getActiveScene();
		if(scene)
		{
			PH_DEFAULT_LOG(
				"save scene: no file name specified, default to {}", scene->getName());
			optItemPath = Path(scene->getName());
		}
		else
		{
			PH_DEFAULT_LOG_WARNING(
				"cannot save scene: file name not specified and there is no active scene");
			return;
		}
	}

	PH_ASSERT(optDirPath);
	PH_ASSERT(optItemPath);

	if(shouldSaveWithFolder)
	{
		Path containingFolder = optDirPath->getTrailingElement();
		if(containingFolder != *optItemPath)
		{
			*optDirPath = *optDirPath / *optItemPath;
		}
	}

	editor.saveScene(*optDirPath / *optItemPath);
}

}// end anonymous namespace

ImguiEditorSceneManager::ImguiEditorSceneManager()
	: m_selectedSceneIdx(static_cast<std::size_t>(-1))
	, m_shouldSaveWithFolder(true)
	, m_newSceneNameBuffer()
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

	{
		ImguiFileSystemDialog& fsDialog = editorUI.getGeneralFileSystemDialog();
		if(ImGui::Button("Open"))
		{
			fsDialog.openPopup(OPEN_SCENE_TITLE);
		}

		fsDialog.buildFileSystemDialogPopupModal(
			OPEN_SCENE_TITLE, 
			editorUI,
			get_file_dialog_size(editor));

		if(fsDialog.selectionConfirmed())
		{
			auto items = fsDialog.getSelectedItems();
			for(const auto& item : items)
			{
				PH_DEFAULT_LOG("selected: {}", item.toString());
			}
		}

		// TODO
	}
	
	{
		ImguiFileSystemDialog& fsDialog = editorUI.getGeneralFileSystemDialog();
		if(ImGui::Button("Save"))
		{
			fsDialog.openPopup(SAVE_SCENE_TITLE);
		}

		ImGui::SameLine();
		ImGui::Checkbox("With Folder", &m_shouldSaveWithFolder);

		fsDialog.buildFileSystemDialogPopupModal(
			SAVE_SCENE_TITLE, 
			editorUI,
			get_file_dialog_size(editor),
			{.canSelectItem = true, .canSelectDirectory = true, .requiresDirectorySelection = true});

		if(fsDialog.selectionConfirmed())
		{
			save_scene(editor, fsDialog, m_shouldSaveWithFolder);
		}
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
