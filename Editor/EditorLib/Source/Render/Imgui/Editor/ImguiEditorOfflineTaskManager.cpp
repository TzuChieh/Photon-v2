#include "Render/Imgui/Editor/ImguiEditorOfflineTaskManager.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorOfflineTaskManager::ImguiEditorOfflineTaskManager(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)
	
	, m_taskInfos()
	, m_numValidTaskInfos(0)
	, m_selectedTaskInfoIdx(static_cast<std::size_t>(-1))
{}

void ImguiEditorOfflineTaskManager::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	// Left child: Task infos
	ImGui::BeginChild(
		"task_infos_panel", 
		ImVec2(
			std::max(editor.dimensionHints.propertyPanelMinWidth, ImGui::GetContentRegionAvail().x * 0.2f),
			0),
		true, 
		ImGuiWindowFlags_HorizontalScrollbar);
	buildTaskInfoContent(editor.getActiveScene());
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: Details of the task
	ImGui::BeginChild(
		"task_details_panel",
		ImVec2(0, 0),
		true);
	buildTaskDetailContent();
	ImGui::EndChild();

	ImGui::End();
}

auto ImguiEditorOfflineTaskManager::getAttributes() const
-> Attributes
{
	return {
		.title = "Offline Task Manager",
		.icon = PH_IMGUI_TASK_MANAGER_ICON,
		.tooltip = "Offline Task Manager",
		.preferredDockingLot = EImguiPanelDockingLot::Bottom,
		.useSidebar = true};
}

void ImguiEditorOfflineTaskManager::buildTaskInfoContent(DesignerScene* scene)
{
	m_numValidTaskInfos = 0;
	if(scene)
	{
		if(m_taskInfos.size() < scene->getRendererBindings().size())
		{
			m_taskInfos.resize(scene->getRendererBindings().size());
		}

		for(const DesignerRendererBinding& binding : scene->getRendererBindings())
		{
			TaskInfo& info = m_taskInfos[m_numValidTaskInfos];
			info.ownerObj = binding.ownerObj;
			info.renderer = binding.offlineRenderer;

			++m_numValidTaskInfos;
		}
	}

	// List box for all offline tasks gathered
	// Custom size: use all available width & height
	if(ImGui::BeginListBox("##scenes_listbox", ImVec2(-FLT_MIN, -FLT_MIN)))
	{
		for(std::size_t ti = 0; ti < m_numValidTaskInfos; ++ti)
		{
			TaskInfo& info = m_taskInfos[ti];

			const bool isSelected = (ti == m_selectedTaskInfoIdx);
			if(ImGui::Selectable(
				info.ownerObj ? info.ownerObj->getName().c_str() : "(no name)", 
				isSelected))
			{
				m_selectedTaskInfoIdx = ti;
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

void ImguiEditorOfflineTaskManager::buildTaskDetailContent()
{
	if(m_numValidTaskInfos == 0)
	{
		ImGui::TextUnformatted("Currently there is no offline rendering task.");
		return;
	}
}

}// end namespace ph::editor
