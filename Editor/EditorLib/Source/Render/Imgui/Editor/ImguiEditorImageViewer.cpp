#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"

#include "ThirdParty/DearImGui.h"

#include <algorithm>

namespace ph::editor
{

ImguiEditorImageViewer::ImguiEditorImageViewer(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_imageNames()
	, m_currentImageNameIdx(static_cast<std::size_t>(-1))
{}

ImguiEditorImageViewer::~ImguiEditorImageViewer()
{}

void ImguiEditorImageViewer::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	const ImGuiStyle& style = ImGui::GetStyle();
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	// Draw image on the entire content region, without padding
	ImVec2 imageAreaMin = ImGui::GetWindowContentRegionMin();
	imageAreaMin.x -= style.WindowPadding.x * 0.5f;
	imageAreaMin.y -= style.WindowPadding.y * 0.5f;
	ImGui::SetCursorPos(imageAreaMin);

	if(m_currentImageNameIdx < m_imageNames.size())
	{
		imageLib.imguiImage(m_imageNames[m_currentImageNameIdx].c_str(), {300, 300});
	}

	
	buildTopToolbar();
	

	ImGui::End();
}

auto ImguiEditorImageViewer::getAttributes() const
-> Attributes
{
	return {
		.title = "Image Viewer",
		.icon = PH_IMGUI_IMAGE_ICON,
		.tooltip = "Image Viewer",
		.preferredDockingLot = EImguiPanelDockingLot::Center,
		.useSidebar = true};
}

void ImguiEditorImageViewer::buildTopToolbar()
{
	const ImGuiStyle& style = ImGui::GetStyle();
	ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	constexpr float moreAlpha = 0.3f;

	auto frameBgColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
	frameBgColor.w = std::min(frameBgColor.w + moreAlpha, 1.0f);
	auto frameBgHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);
	frameBgHoveredColor.w = std::min(frameBgHoveredColor.w + moreAlpha, 1.0f);
	auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
	buttonColor.x *= 0.5f;
	buttonColor.y *= 0.5f;
	buttonColor.z *= 0.5f;
	buttonColor.w = std::min(buttonColor.w + moreAlpha, 1.0f);

	// Reset to start position so we can draw on top of the image
	ImGui::SetCursorPos(ImGui::GetCursorStartPos());

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgHoveredColor);
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
	ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 1});
	
	constexpr const char* defaultName = "(no loaded image)";
	const auto defaultNameSize = ImGui::CalcTextSize(defaultName);

	ImGui::SetNextItemWidth(defaultNameSize.x * 2 + style.FramePadding.x * 2);
	if(ImGui::BeginCombo(
		"##image_names_combo", 
		m_currentImageNameIdx < m_imageNames.size() ? m_imageNames[m_currentImageNameIdx].c_str() : defaultName))
	{
		for(std::size_t ni = 0; ni < m_imageNames.size(); ++ni)
		{
			const bool isSelected = (ni == m_currentImageNameIdx);
			if(ImGui::Selectable(m_imageNames[ni].c_str(), isSelected))
			{
				m_currentImageNameIdx = ni;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if(ImGui::Button(PH_IMGUI_OPEN_FILE_ICON))
	{
		fsDialog.openPopup(ImguiFileSystemDialog::OPEN_FILE_TITLE);
	}

	ImGui::SameLine();

	if(ImGui::Button(PH_IMGUI_CROSS_ICON))
	{
		// TODO
	}

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(3);
	

	fsDialog.buildFileSystemDialogPopupModal(
		ImguiFileSystemDialog::OPEN_FILE_TITLE,
		getEditorUI());

	if(fsDialog.dialogClosed())
	{
		if(!fsDialog.getSelectedItem().isEmpty())
		{
			Path imageFile = fsDialog.getSelectedTarget();
			auto imageName = imageFile.toAbsoluteString();
			if(!imageFile.isEmpty() && !imageLib.has(imageName))
			{
				imageLib.loadImage(imageName, imageFile);
				m_imageNames.push_back(imageName);
			}
		}
	}
}

}// end namespace ph::editor
