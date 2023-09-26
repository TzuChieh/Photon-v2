#include "Render/Imgui/Editor/ImguiEditorOfflineTaskManager.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/Render/RenderAgent.h"
#include "Designer/Render/RenderConfig.h"
#include "Render/Renderer/OfflineRenderer.h"

#include "ThirdParty/DearImGui.h"

#include <cstdio>
#include <array>
#include <cinttypes>

namespace ph::editor
{

ImguiEditorOfflineTaskManager::ImguiEditorOfflineTaskManager(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)
	
	, m_taskInfos()
	, m_numValidTaskInfos(0)
	, m_selectedTaskInfoIdx(0)
	, m_enableStopRender(false)
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

	if(!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		// Safeguarding ongoing render task
		m_enableStopRender = false;
	}

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
			info.agent = binding.agent;
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
				info.agent ? info.agent->getName().c_str() : "(no name)",
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

	TaskInfo& info = m_taskInfos[m_selectedTaskInfoIdx];
	if(info.renderer)
	{
		info.stage = info.renderer->getRenderStage();
		info.renderer->tryGetRenderStats(&info.stats);
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.16f, 0.16f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.14f, 0.14f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	const bool isTaskRunning = info.stage != render::EOfflineRenderStage::Finished;

	if(isTaskRunning) { ImGui::BeginDisabled(); }
	if(ImGui::Button(PH_IMGUI_START_ICON PH_IMGUI_ICON_TIGHT_PADDING "Start Render "))
	{
		if(info.agent && info.renderer)
		{
			info.renderer->render(info.agent->getRenderConfig());
		}
		else
		{
			PH_LOG_ERROR(DearImGui,
				"Cannot start render, required target missing (agent={}, renderer={}).",
				static_cast<void*>(info.agent), static_cast<void*>(info.renderer));
		}
	}
	if(isTaskRunning) { ImGui::EndDisabled(); }

	ImGui::SameLine();
	if(!m_enableStopRender) { ImGui::BeginDisabled(); }
	if(ImGui::Button(PH_IMGUI_STOP_ICON PH_IMGUI_ICON_TIGHT_PADDING "Stop Render "))
	{
		// TODO
	}
	if(!m_enableStopRender) { ImGui::EndDisabled(); }

	ImGui::SameLine();
	ImGui::Checkbox("Enable Stop Render", &m_enableStopRender);

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::TextUnformatted("|");
	ImGui::SameLine();
	ImGui::TextUnformatted("Renderer Status:");

	const char* stageName = "Unknown";
	switch(info.stage)
	{
	case render::EOfflineRenderStage::Standby: stageName = "Standby"; break;
	case render::EOfflineRenderStage::CopyingScene: stageName = "Copying Scene"; break;
	case render::EOfflineRenderStage::LoadingScene: stageName = "Loading Scene"; break;
	case render::EOfflineRenderStage::Updating: stageName = "Updating"; break;
	case render::EOfflineRenderStage::Developing: stageName = "Developing"; break;
	case render::EOfflineRenderStage::Finished: stageName = "Finished"; break;
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(stageName);

	const float progress = info.stats.totalWork != 0
		? static_cast<float>(info.stats.workDone) / static_cast<float>(info.stats.totalWork)
		: 0.0f;

	std::array<char, 32> progressOverlay;
	std::snprintf(
		progressOverlay.data(),
		progressOverlay.size(),
		"Progress: %.3f%%",
		progress * 100.0f);
	ImGui::ProgressBar(progress, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), progressOverlay.data());

	ImGui::Separator();

	ImGui::BeginGroup();
	ImGui::TextUnformatted("Layers:");
	if(info.stats.layerNames.empty())
	{
		ImGui::TextUnformatted("No layer name available.");
	}
	else
	{
		for(const std::string& name : info.stats.layerNames)
		{
			ImGui::BulletText(name.c_str());
		}
	}
	ImGui::EndGroup();

	ImGui::SameLine();
	ImGui::Spacing();

	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Statistics:");
	if(info.stats.numericInfos.empty())
	{
		ImGui::TextUnformatted("No statistics available.");
	}
	else
	{
		for(const auto& numericInfo : info.stats.numericInfos)
		{
			if(numericInfo.isInteger)
			{
				ImGui::BulletText("%s: %" PRId64, numericInfo.name.c_str(), static_cast<int64>(numericInfo.value));
			}
			else
			{
				ImGui::BulletText("%s: %f", numericInfo.value);
			}
		}
	}
	ImGui::EndGroup();
}

}// end namespace ph::editor
