#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include <algorithm>

namespace ph::editor
{

ImguiEditorImageViewer::ImguiEditorImageViewer(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_imageStates()
	, m_currentImageIdx(static_cast<std::size_t>(-1))
	, m_imageInfoBuffer(256, '\0')
{}

ImguiEditorImageViewer::~ImguiEditorImageViewer()
{}

void ImguiEditorImageViewer::buildWindow(const char* windowIdName, bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_NoScrollbar;

	if(!ImGui::Begin(windowIdName, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	const ImGuiStyle& style = ImGui::GetStyle();
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	ImVec2 contentRegionMin = ImGui::GetCursorPos();
	ImVec2 contentRegionMax = ImGui::GetContentRegionAvail();

	// Place image on a reduced-padding content region
	math::Vector2F imageAreaMin(contentRegionMin.x, contentRegionMin.y);
	math::Vector2F imageAreaMax(contentRegionMax.x, contentRegionMax.y);
	imageAreaMin -= style.WindowPadding.x * 0.5f;
	imageAreaMax += style.WindowPadding.y * 0.5f;

	// Populate image states for newly loaded image
	if(m_currentImageIdx < m_imageStates.size())
	{
		ImageState& state = m_imageStates[m_currentImageIdx];
		if(!state.textureID)
		{
			const ImTextureID textureID = imageLib.get(state.name);
			const bool isNewlyLoaded = textureID && !state.textureID;
			if(isNewlyLoaded)
			{
				state.textureID = textureID;
				state.textureFormat = imageLib.getFormat(state.name);
				state.actualSize = math::Vector2F(imageLib.getSizePx(state.name));
				state.sizeInWindow = state.actualSize;

				// Center the image if it is newly loaded
				state.minPosInWindow = imageAreaMin + ((imageAreaMax - imageAreaMin) - state.actualSize) * 0.5f;
			}
		}

		ImGui::SetCursorPos({state.minPosInWindow.x(), state.minPosInWindow.y()});
		imgui::image_with_fallback(
			state.textureID,
			{state.sizeInWindow.x(), state.sizeInWindow.y()});
	}
	
	buildTopToolbar();
	buildBottomToolbar();
	

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

	// Reset to start position so we can draw on top of the image
	ImGui::SetCursorPos(ImGui::GetCursorStartPos());
	
	constexpr const char* defaultName = "(no loaded image)";
	const auto defaultNameSize = ImGui::CalcTextSize(defaultName);

	pushToolbarStyleAndColor();

	ImGui::SetNextItemWidth(defaultNameSize.x * 2 + style.FramePadding.x * 2);
	if(ImGui::BeginCombo(
		"##image_names_combo", 
		m_currentImageIdx < m_imageStates.size() ? m_imageStates[m_currentImageIdx].name.c_str() : defaultName))
	{
		for(std::size_t ni = 0; ni < m_imageStates.size(); ++ni)
		{
			const bool isSelected = (ni == m_currentImageIdx);
			if(ImGui::Selectable(m_imageStates[ni].name.c_str(), isSelected))
			{
				m_currentImageIdx = ni;
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

	//ImGui::SameLine();
	/*ImGui::Dummy({tightSpacing, 1});
	ImGui::SameLine();
	static char aaa[64];
	ImGui::InputText("##tetetet", aaa, 64);*/

	popToolbarStyleAndColor();
	

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
				m_imageStates.push_back({
					.name = imageName});
			}
		}
	}
}

void ImguiEditorImageViewer::buildBottomToolbar()
{
	const ImGuiStyle& style = ImGui::GetStyle();
	ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	// Reset to start position first so we can use `GetContentRegionAvail()` to find the bottom
	const ImVec2 cursorStartPos = ImGui::GetCursorStartPos();
	ImGui::SetCursorPos(cursorStartPos);

	// Reset to the lower left position so we can draw on top of the image
	math::Vector2F lowerLeftPos(cursorStartPos.x, cursorStartPos.y);
	lowerLeftPos.y() += ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight();
	ImGui::SetCursorPos({lowerLeftPos.x(), lowerLeftPos.y()});

	pushToolbarStyleAndColor();

	auto size = math::Vector2F{0, 0};
	auto format = ghi::ESizedPixelFormat::Empty;
	float scale = 1;
	auto pointedPixelPos = math::Vector2F{0, 0};

	if(m_currentImageIdx < m_imageStates.size())
	{
		ImageState& state = m_imageStates[m_currentImageIdx];
		size = state.actualSize;
		format = state.textureFormat;
		scale = state.actualSize.x() > 0 ? state.sizeInWindow.x() / state.actualSize.x() : 1;

		if(ImGui::IsWindowHovered() && state.sizeInWindow.x() > 0 && state.sizeInWindow.y() > 0)
		{
			const ImVec2 windowAbsPos = ImGui::GetWindowPos();
			const ImVec2 mouseAbsPos = ImGui::GetMousePos();

			math::Vector2F mousePosInWindow(
				mouseAbsPos.x - windowAbsPos.x, mouseAbsPos.y - windowAbsPos.y);
			pointedPixelPos = 
				(mousePosInWindow - state.minPosInWindow) / state.sizeInWindow * state.actualSize;
		}
	}

	const char* imageFormat = "Unknown";
	switch(format)
	{
	case ghi::ESizedPixelFormat::RGB_8: imageFormat = "RGB, 8-bit channels"; break;
	case ghi::ESizedPixelFormat::RGBA_8: imageFormat = "RGBA, 8-bit channels"; break;
	case ghi::ESizedPixelFormat::RGB_16F: imageFormat = "RGB, 16-bit channels"; break;
	case ghi::ESizedPixelFormat::RGBA_16F: imageFormat = "RGBA, 16-bit channels"; break;
	case ghi::ESizedPixelFormat::RGB_32F: imageFormat = "RGB, 32-bit channels"; break;
	case ghi::ESizedPixelFormat::RGBA_32F: imageFormat = "RGBA, 32-bit channels"; break;
	}

	std::snprintf(
		m_imageInfoBuffer.data(),
		m_imageInfoBuffer.size(),
		"Size: %5.0f, %5.0f | Format: %s | Scale: %6.3f | Pixel: %7.2f, %7.2f",
		size.x(), size.y(), imageFormat, scale, pointedPixelPos.x(), pointedPixelPos.y());

	constexpr ImGuiInputTextFlags inputTextFlags =
		ImGuiInputTextFlags_ReadOnly;

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText(
		"##image_info", 
		m_imageInfoBuffer.data(), 
		m_imageInfoBuffer.size(),
		inputTextFlags);

	popToolbarStyleAndColor();
}

void ImguiEditorImageViewer::pushToolbarStyleAndColor()
{
	constexpr float moreAlpha = 0.3f;
	constexpr float tightSpacing = 3.0f;

	auto frameBgColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
	frameBgColor.w = std::min(frameBgColor.w + moreAlpha, 1.0f);
	auto frameBgHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);
	frameBgHoveredColor.w = std::min(frameBgHoveredColor.w + moreAlpha, 1.0f);
	auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
	buttonColor.x *= 0.5f;
	buttonColor.y *= 0.5f;
	buttonColor.z *= 0.5f;
	buttonColor.w = std::min(buttonColor.w + moreAlpha, 1.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(tightSpacing, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgHoveredColor);
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
	ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 1});
}

void ImguiEditorImageViewer::popToolbarStyleAndColor()
{
	// Must kept in sync with `pushToolbarStyleAndColor()`

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(3);
}

}// end namespace ph::editor
