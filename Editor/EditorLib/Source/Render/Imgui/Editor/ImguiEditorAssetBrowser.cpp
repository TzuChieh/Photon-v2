#include "Render/Imgui/Editor/ImguiEditorAssetBrowser.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"

#include "ThirdParty/DearImGui.h"

#include <algorithm>

namespace ph::editor
{

namespace
{

inline const char* get_visibility_icon(const bool isVisible)
{
	return isVisible ? PH_IMGUI_VISIBLE_ICON : PH_IMGUI_INVISIBLE_ICON;
}

inline void build_visibility_toggle_widget(const char* const label, bool& visibilityState)
{
	// When using the same icon font for the button, they will have the same ID and conflict. 
	// Use label for unique ID.
	ImGui::PushID(label);

	if(ImGui::Button(get_visibility_icon(visibilityState)))
	{
		visibilityState = !visibilityState;
	}
	ImGui::SameLine();
	ImGui::Text(label);

	ImGui::PopID();
}

}// end anonymous namespace

ImguiEditorAssetBrowser::ImguiEditorAssetBrowser(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_filterState(ORDINARY_ASSET_ONLY)

	, m_geometryVisibility(true)
	, m_materialVisibility(true)
	, m_motionVisibility(true)
	, m_imageVisibility(true)

	, m_sampleSourceVisibility(true)
	, m_visualizerVisibility(true)
	, m_optionVisibility(true)
{}

void ImguiEditorAssetBrowser::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();

	// Left child: asset visibility control & filter
	ImGui::BeginChild(
		"asset_browser_control_panel", 
		ImVec2(
			std::max(editor.dimensionHints.propertyPanelMinWidth, ImGui::GetContentRegionAvail().x * 0.2f),
			0),
		true, 
		ImGuiWindowFlags_HorizontalScrollbar);
	buildControlPanelContent();
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: asset item view
	ImGui::BeginChild(
		"asset_browser_item_view",
		ImVec2(0, 0),
		true);
	buildItemViewContent();
	ImGui::EndChild();

	ImGui::End();
}

auto ImguiEditorAssetBrowser::getAttributes() const
-> Attributes
{
	return {
		.title = "Asset Browser",
		.icon = PH_IMGUI_ASSET_ICON,
		.preferredDockingLot = EImguiPanelDockingLot::Bottom,
		.isOpenedByDefault = true,
		.isCloseable = false};
}

void ImguiEditorAssetBrowser::buildControlPanelContent()
{
	ImGui::RadioButton("Ordinary", &m_filterState, ORDINARY_ASSET_ONLY);
	ImGui::SameLine();
	ImGui::RadioButton("Core", &m_filterState, CORE_ASSET_ONLY);

	if(m_filterState == ORDINARY_ASSET_ONLY)
	{
		build_visibility_toggle_widget("Geometry", m_geometryVisibility);
		build_visibility_toggle_widget("Material", m_materialVisibility);
		build_visibility_toggle_widget("Motion", m_motionVisibility);
		build_visibility_toggle_widget("Image", m_imageVisibility);
	}
	else if(m_filterState == CORE_ASSET_ONLY)
	{
		build_visibility_toggle_widget("Sample Source", m_sampleSourceVisibility);
		build_visibility_toggle_widget("Visualizer", m_visualizerVisibility);
		build_visibility_toggle_widget("Option", m_optionVisibility);
	}
}

void ImguiEditorAssetBrowser::buildItemViewContent()
{
	// TODO
}

}// end namespace ph::editor
