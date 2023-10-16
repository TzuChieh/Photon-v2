#include "Render/Imgui/Editor/ImguiEditorSceneCreator.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/ImguiEditorTheme.h"
#include "Render/Imgui/Utility/imgui_helpers.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include "ThirdParty/DearImGui.h"

#include <Common/logging.h>

#include <cstring>

namespace ph::editor
{

ImguiEditorSceneCreator::ImguiEditorSceneCreator(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_sceneNameBuffer(128, '\0')
	, m_withInitialSceneDescription(false)
	, m_initialSceneDescription()
	, m_initialContentSummary()
	, m_baseWorkingDirectory()
	, m_composedWorkingDirectory()
	, m_workingDirectoryPreview()
	, m_unsatisfactionMessage()
	, m_withContainingFolder(true)
{
	imgui::copy_to(m_sceneNameBuffer, DesignerScene::defaultSceneName());

	summarizeInitialContent();
	composeSceneWorkingDirectory();
}

void ImguiEditorSceneCreator::buildWindow(const char* windowIdName, bool* isOpening)
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if(!ImGui::Begin(windowIdName, isOpening, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	ImGui::BulletText("Fill in basic information");
	ImGui::Spacing();
	if(ImGui::InputText("Scene Name", m_sceneNameBuffer.data(), m_sceneNameBuffer.size()))
	{
		composeSceneWorkingDirectory();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::BulletText("Add initial content");
	ImGui::Spacing();
	if(ImGui::Checkbox("With Scene Description", &m_withInitialSceneDescription))
	{
		summarizeInitialContent();
	}
	
	// Scene description selection
	{
		ImguiFileSystemDialog& selectSceneDescription = getEditorUI().getGeneralFileSystemDialog();
		if(!m_withInitialSceneDescription) { ImGui::BeginDisabled(); }
		if(ImGui::Button("Select Scene Description"))
		{
			selectSceneDescription.openPopup(ImguiFileSystemDialog::OPEN_FILE_TITLE);
		}
		if(!m_withInitialSceneDescription) { ImGui::EndDisabled(); }

		selectSceneDescription.buildFileSystemDialogPopupModal(
			ImguiFileSystemDialog::OPEN_FILE_TITLE, getEditorUI());
		if(selectSceneDescription.dialogClosed())
		{
			if(!selectSceneDescription.getSelectedItem().isEmpty())
			{
				m_initialSceneDescription = selectSceneDescription.getSelectedTarget();
			}

			summarizeInitialContent();
		}
	}
	
	ImGui::Spacing();
	ImGui::Text("Initial Content:");
	ImGui::BeginDisabled();
	ImGui::TextWrapped(m_initialContentSummary.c_str());
	ImGui::EndDisabled();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::BulletText("Where to save?");
	ImGui::Spacing();
	{
		ImguiFileSystemDialog& selectBaseWorkingDirectory = getEditorUI().getGeneralFileSystemDialog();
		if(ImGui::Button("Select Scene Working Directory"))
		{
			m_baseWorkingDirectory.clear();
			m_composedWorkingDirectory.clear();
			m_workingDirectoryPreview.clear();

			selectBaseWorkingDirectory.openPopup(ImguiFileSystemDialog::OPEN_FOLDER_TITLE);
		}

		selectBaseWorkingDirectory.buildFileSystemDialogPopupModal(
			ImguiFileSystemDialog::OPEN_FOLDER_TITLE,
			getEditorUI(),
			{.canSelectItem = false, .canSelectDirectory = true, .requiresDirectorySelection = true});

		if(ImGui::Checkbox("With Containing Folder", &m_withContainingFolder))
		{
			composeSceneWorkingDirectory();
		}

		if(selectBaseWorkingDirectory.dialogClosed())
		{
			m_baseWorkingDirectory = selectBaseWorkingDirectory.getSelectedDirectory();

			composeSceneWorkingDirectory();
		}

		ImGui::Spacing();
		ImGui::Text("Directory Preview:");
		ImGui::BeginDisabled();
		ImGui::TextWrapped(m_workingDirectoryPreview.c_str());
		ImGui::EndDisabled();
	}
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if(!m_unsatisfactionMessage.empty())
	{
		ImGui::Spacing();
		ImGui::TextColored(getEditorUI().getTheme().warningColor, m_unsatisfactionMessage.c_str());
		ImGui::Spacing();
	}

	if(ImGui::Button("OK"))
	{
		m_unsatisfactionMessage.clear();

		if(std::strlen(m_sceneNameBuffer.data()) == 0)
		{
			m_unsatisfactionMessage += "* Please specify a scene name.\n";
		}

		if(m_baseWorkingDirectory.isEmpty())
		{
			m_unsatisfactionMessage += "* Please specify a scene working directory.\n";
		}

		const bool canCreateScene = m_unsatisfactionMessage.empty();
		if(canCreateScene)
		{
			composeSceneWorkingDirectory();
			editor.createScene(m_composedWorkingDirectory, m_sceneNameBuffer.data());

			if(isOpening)
			{
				*isOpening = false;
			}
		}
	}
	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if(ImGui::Button("Cancel"))
	{
		m_unsatisfactionMessage.clear();

		if(isOpening)
		{
			*isOpening = false;
		}
	}

	ImGui::End();
}

auto ImguiEditorSceneCreator::getAttributes() const
-> Attributes
{
	return {
		.title = "Scene Creator",
		.icon = PH_IMGUI_SCENE_CREATION_ICON,
		.tooltip = "Create New Scene",
		.category = EImguiPanelCategory::File};
}

void ImguiEditorSceneCreator::summarizeInitialContent()
{
	m_initialContentSummary.clear();
	if(m_withInitialSceneDescription && !m_initialSceneDescription.isEmpty())
	{
		m_initialContentSummary += "Initial scene description: \"";
		m_initialContentSummary += m_initialSceneDescription.toString();
		m_initialContentSummary += "\". ";
	}
	else
	{
		m_initialContentSummary += "No initial scene description. ";
	}
}

void ImguiEditorSceneCreator::composeSceneWorkingDirectory()
{
	if(m_baseWorkingDirectory.isEmpty())
	{
		m_workingDirectoryPreview = "(no directory specified)";
		return;
	}

	m_composedWorkingDirectory = m_baseWorkingDirectory;

	// Optionally add a folder that has the same name as the scene
	if(m_withContainingFolder)
	{
		Path containingFolder = m_baseWorkingDirectory.getTrailingElement();
		std::string sceneName(m_sceneNameBuffer.data());
		if(containingFolder.toString() != sceneName)
		{
			m_composedWorkingDirectory = m_baseWorkingDirectory / sceneName;
		}
	}

	m_workingDirectoryPreview = m_composedWorkingDirectory.toAbsoluteString();
}

}// end namespace ph::editor
