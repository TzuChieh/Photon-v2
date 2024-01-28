#include "Render/Imgui/Editor/ImguiEditorOfflineTaskManager.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/Render/RenderAgent.h"
#include "Designer/Render/RenderConfig.h"
#include "Render/Renderer/OfflineRenderer.h"
#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"

#include "ThirdParty/DearImGui.h"

#include <Math/time.h>

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
	, m_enableRenderPreview(true)
	, m_indicateUpdatedRegions(true)
	, m_autoSyncImageView(true)
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
	// Collect and initialize task infos every frame
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

			// Image name should be unique over all scenes, as we use it for the image viewer
			if(info.agent)
			{
				info.outputImageName.resize(info.agent->getName().size() + 32);
				const int numChars = std::snprintf(
					info.outputImageName.data(),
					info.outputImageName.size(),
					"%s <ID: %" PRIu64 ">",
					info.agent->getName().c_str(),
					info.agent->getId());

				PH_ASSERT(0 < numChars && numChars < info.outputImageName.size());
				info.outputImageName.resize(numChars);
			}
			else
			{
				info.outputImageName.clear();
			}

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

				// Sync image view when task is clicked
				if(m_autoSyncImageView)
				{
					getEditorUI().getImageViewer().setCurrentImage(info.outputImageName);
				}
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
	}

	const bool isTaskRunning = info.stage != render::EOfflineRenderStage::Finished;
	if(isTaskRunning)
	{
		info.renderer->tryGetRenderStats(&info.stats);

		if(m_enableRenderPreview && info.stats.viewport.hasView())
		{
			ImguiEditorImageViewer& viewer = getEditorUI().getImageViewer();

			const bool hasNoImage = !viewer.hasImage(info.outputImageName);

			// The (0, 0) check is necessary, so we can be sure that the image is in fact having
			// another size rather than still being processed (not ready).
			const bool hasWrongImageSize = 
				viewer.getImageSizePx(info.outputImageName) != math::Vector2UI(0, 0) &&
				viewer.getImageSizePx(info.outputImageName) != info.stats.viewport.getBaseSizePx();

			// Update image buffer if not exist or size mismatch
			if(hasNoImage || hasWrongImageSize)
			{
				viewer.removeImage(info.outputImageName);
				viewer.addImage(info.outputImageName, info.stats.viewport.getBaseSizePx());

				// Sync image view when image buffer updated
				if(m_autoSyncImageView)
				{
					getEditorUI().getImageViewer().setCurrentImage(info.outputImageName);
				}
			}

			// Update image handle if changed
			auto freshHandle = viewer.getImageGraphicsHandle(info.outputImageName);
			if(freshHandle != info.peek.in.resultHandle)
			{
				info.peek.in.resultHandle = freshHandle;
			}

			info.peek.in.wantIntermediateResult = m_enableRenderPreview;
			info.peek.in.wantUpdatingRegions = m_indicateUpdatedRegions;
			if(info.renderer->tryGetRenderPeek(&info.peek, true))
			{
				if(m_indicateUpdatedRegions)
				{
					viewer.setImagePixelIndicators(info.outputImageName, info.peek.out.updatingRegions);
				}
			}
		}
	}
	else
	{
		// Clear update indicators if not rendering
		ImguiEditorImageViewer& viewer = getEditorUI().getImageViewer();
		viewer.setImagePixelIndicators(info.outputImageName, {});
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.16f, 0.16f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.14f, 0.14f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	if(isTaskRunning) { ImGui::BeginDisabled(); }
	if(ImGui::Button(PH_IMGUI_START_ICON PH_IMGUI_ICON_TIGHT_PADDING "Start Render "))
	{
		if(info.agent && info.renderer)
		{
			info.agent->renderWithDefaultConfig();

			// Sync image view when rendering start
			if(m_autoSyncImageView)
			{
				getEditorUI().getImageViewer().setCurrentImage(info.outputImageName);
			}
		}
		else
		{
			PH_LOG(DearImGui, Error,
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
	case render::EOfflineRenderStage::Rendering: stageName = "Rendering"; break;
	case render::EOfflineRenderStage::Developing: stageName = "Developing"; break;
	case render::EOfflineRenderStage::Finished: stageName = "Finished"; break;
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(stageName);

	const float progress = info.stats.totalWork != 0
		? static_cast<float>(info.stats.workDone) / static_cast<float>(info.stats.totalWork) : 0.0f;

	std::array<char, 32> progressOverlay;
	std::snprintf(
		progressOverlay.data(),
		progressOverlay.size(),
		"Progress: %.3f%%",
		progress * 100.0f);
	ImGui::ProgressBar(progress, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), progressOverlay.data());

	ImGui::Separator();

	const auto& baseSize = info.stats.viewport.getBaseSizePx();
	const auto& cropRegion = info.stats.viewport.getCroppedRegionPx();

	// Group 1: Task settings
	ImGui::BeginGroup();
	ImGui::Checkbox("Enable Render Preview", &m_enableRenderPreview);
	if(!m_enableRenderPreview) { ImGui::BeginDisabled(); }
	ImGui::Checkbox("Indicate Updated Regions", &m_indicateUpdatedRegions);
	if(!m_enableRenderPreview) { ImGui::EndDisabled(); }
	ImGui::Checkbox("Auto Sync Image View", &m_autoSyncImageView);
	ImGui::EndGroup();

	ImGui::SameLine();
	ImGui::Spacing();

	// Group 2: Static information about this render
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Text(
		"Full Viewport: (%d, %d), Render Min: (%d, %d) Max: (%d, %d)",
		static_cast<int>(baseSize.x()), static_cast<int>(baseSize.y()),
		static_cast<int>(cropRegion.getMinVertex().x()), static_cast<int>(cropRegion.getMinVertex().y()),
		static_cast<int>(cropRegion.getMaxVertex().x()), static_cast<int>(cropRegion.getMaxVertex().y()));
	ImGui::TextUnformatted("Layers:");
	if(info.stats.layerNames.empty())
	{
		ImGui::BulletText("No layer name available.");
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

	// Group 3: Dynamic information (numeric statistics)
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Statistics:");
	if(info.stats.numericInfos.empty())
	{
		ImGui::BulletText("No statistics available.");
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
				ImGui::BulletText("%s: %f", numericInfo.name.c_str(), numericInfo.value);
			}
		}
	}

	auto renderTimeHMS = math::milliseconds_to_HMS(info.stats.renderTimeMs);
	ImGui::Text("Render Time: %02d:%02d:%02d",
		static_cast<int>(renderTimeHMS[0]),
		static_cast<int>(renderTimeHMS[1]),
		static_cast<int>(renderTimeHMS[2]));
	
	const uint64 estimatedTotalMs = progress > 0.0f ? 
		static_cast<uint64>(info.stats.renderTimeMs / progress) : 0;
	auto timeLeftHMS = math::milliseconds_to_HMS(estimatedTotalMs - info.stats.renderTimeMs);
	ImGui::Text("Time Left: %02d:%02d:%02d",
		static_cast<int>(timeLeftHMS[0]),
		static_cast<int>(timeLeftHMS[1]),
		static_cast<int>(timeLeftHMS[2]));

	ImGui::EndGroup();
}

}// end namespace ph::editor
