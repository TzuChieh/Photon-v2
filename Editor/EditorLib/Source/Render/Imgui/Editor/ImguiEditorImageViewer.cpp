#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "App/Misc/EditorSettings.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph::editor
{

ImguiEditorImageViewer::ImguiEditorImageViewer(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_imageStates()
	, m_currentImageIdx(static_cast<std::size_t>(-1))
	, m_imageInfoBuffer(256, '\0')
	, m_lastMouseDragDelta(0, 0)
	, m_viewAreaMin()
	, m_viewAreaSize()
	, m_showIndicators(true)
{}

ImguiEditorImageViewer::~ImguiEditorImageViewer()
{}

void ImguiEditorImageViewer::buildWindow(const char* windowIdName, bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	if(!ImGui::Begin(windowIdName, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiIO& io = ImGui::GetIO();
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	const ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

	// View area is a padding-reduced content region
	math::Vector2F viewAreaMin = {contentRegionMin.x, contentRegionMin.y};
	math::Vector2F viewAreaMax = {contentRegionMax.x, contentRegionMax.y};
	viewAreaMin -= style.WindowPadding.x * 0.5f;
	viewAreaMax += style.WindowPadding.y * 0.5f;

	m_viewAreaMin = viewAreaMin;
	m_viewAreaSize = viewAreaMax - viewAreaMin;

	// Control and update the states of current image
	if(hasSelectedImage())
	{
		// Populate image states for newly loaded image
		ImageState& state = getSelectedImageState();
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
				state.minPosInWindow = m_viewAreaMin + (m_viewAreaSize - state.actualSize) * 0.5f;
			}
		}

		if(ImGui::IsWindowFocused())
		{
			const ImVec2 windowAbsPos = ImGui::GetWindowPos();
			const ImVec2 mouseAbsPos = ImGui::GetMousePos();
			const auto mousePosInWindow = math::Vector2F{
				mouseAbsPos.x - windowAbsPos.x, mouseAbsPos.y - windowAbsPos.y};

			// Update currently hovered pixel coordinates
			if(ImGui::IsWindowHovered() && state.sizeInWindow.x() > 0 && state.sizeInWindow.y() > 0)
			{
				state.pointedPixelPos = 
					(mousePosInWindow - state.minPosInWindow) / state.sizeInWindow * state.actualSize;

				// Flip y as ImGui's origin is on the upper-left corner which is not what we want
				state.pointedPixelPos.y() = state.actualSize.y() - state.pointedPixelPos.y();
			}

			// Mouse just stopped dragging
			if(!ImGui::IsMouseDragging(ImGuiMouseButton_Right) &&
			   (m_lastMouseDragDelta.x() != 0 || m_lastMouseDragDelta.y() != 0))
			{
				// Apply the dragged amount to current image
				state.minPosInWindow += m_lastMouseDragDelta;
				m_lastMouseDragDelta = {0, 0};
			}
			else
			{
				const ImVec2 mouseDragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
				m_lastMouseDragDelta = {mouseDragDelta.x, mouseDragDelta.y};
			}

			// Mouse wheel zooming
			if(ImGui::IsWindowHovered() && io.MouseWheel != 0)
			{
				applyZoomTo(state, io.MouseWheel, mousePosInWindow);
			}
		}

		math::Vector2F draggedMinPosInWindow = state.minPosInWindow;
		draggedMinPosInWindow += m_lastMouseDragDelta;

		ImGui::SetCursorPos({draggedMinPosInWindow.x(), draggedMinPosInWindow.y()});
		imgui::image_with_fallback(
			state.textureID,
			{state.sizeInWindow.x(), state.sizeInWindow.y()});

		if(m_showIndicators && state.actualSize.x() > 0 && state.actualSize.y() > 0)
		{
			const auto pixelToScreenOffset = draggedMinPosInWindow.add({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y});
			const auto pixelToWindowScale = state.sizeInWindow / state.actualSize;
			for(const math::TAABB2D<int32>& rect : state.rectPixelIndicators)
			{
				// Flip y as ImGui's origin is on the upper-left corner which is not what we want
				math::Vector2F rectMin(
					static_cast<float>(rect.getMinVertex().x()), state.actualSize.y() - rect.getMinVertex().y());
				math::Vector2F rectMax(
					static_cast<float>(rect.getMaxVertex().x()), state.actualSize.y() - rect.getMaxVertex().y());

				// Transform to screen coordinates
				rectMin = pixelToScreenOffset + rectMin * pixelToWindowScale;
				rectMax = pixelToScreenOffset + rectMax * pixelToWindowScale;

				// Draw list is using screen coordinates
				ImGui::GetWindowDrawList()->AddRect(
					{rectMin.x(), rectMin.y()},
					{rectMax.x(), rectMax.y()},
					IM_COL32(255, 255, 100, 255),
					0.0f);
			}
		}
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

	// Reset to start position so we can draw on top of the image
	ImGui::SetCursorPos(ImGui::GetCursorStartPos());
	
	constexpr const char* defaultName = "(no selected image)";
	const auto defaultNameSize = ImGui::CalcTextSize(defaultName);

	pushToolbarStyleAndColor();

	ImGui::SetNextItemWidth(defaultNameSize.x * 2 + style.FramePadding.x * 2);
	if(ImGui::BeginCombo(
		"##image_names_combo", 
		hasSelectedImage() ? getSelectedImageState().name.c_str() : defaultName))
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
	if(ImGui::Button(PH_IMGUI_CROSS_ICON) && hasSelectedImage())
	{
		removeImage(getSelectedImageState().name);
	}

	ImGui::SameLine();
	if(ImGui::Button("100%") && hasSelectedImage())
	{
		// Make the image 1:1 size by using its resolution as size in window
		getSelectedImageState().sizeInWindow = getSelectedImageState().actualSize;
	}

	ImGui::SameLine();
	if(ImGui::Button(PH_IMGUI_FOCUS_ICON) && hasSelectedImage())
	{
		ImageState& state = getSelectedImageState();

		const auto viewAreaAspectRatio = m_viewAreaSize.y() > 0
			? m_viewAreaSize.x() / m_viewAreaSize.y() : 1.0f;
		const auto imageAspectRatio = state.sizeInWindow.y() > 0
			? state.sizeInWindow.x() / state.sizeInWindow.y() : 1.0f;

		// Focus the image by letting it extend the full view area (preserving aspect ratio)
		if(viewAreaAspectRatio > imageAspectRatio)
		{
			state.sizeInWindow = {m_viewAreaSize.y() * imageAspectRatio, m_viewAreaSize.y()};
		}
		else
		{
			state.sizeInWindow = {m_viewAreaSize.x(), m_viewAreaSize.x() / imageAspectRatio};
		}

		// Also centering it
		state.minPosInWindow = m_viewAreaMin + (m_viewAreaSize - state.sizeInWindow) * 0.5f;
	}

	ImGui::SameLine();
	if(ImGui::Button(PH_IMGUI_ZOOM_IN_ICON) && hasSelectedImage())
	{
		const ImVec2 windowSize = ImGui::GetWindowSize();
		applyZoomTo(getSelectedImageState(), 1.0f, {windowSize.x * 0.5f, windowSize.y * 0.5f});
	}

	ImGui::SameLine();
	if(ImGui::Button(PH_IMGUI_ZOOM_OUT_ICON) && hasSelectedImage())
	{
		const ImVec2 windowSize = ImGui::GetWindowSize();
		applyZoomTo(getSelectedImageState(), -1.0f, {windowSize.x * 0.5f, windowSize.y * 0.5f});
	}

	ImGui::SameLine();
	if(ImGui::Button(m_showIndicators ? PH_IMGUI_PEN_ICON : PH_IMGUI_NO_PEN_ICON))
	{
		m_showIndicators = !m_showIndicators;
	}

	popToolbarStyleAndColor();

	fsDialog.buildFileSystemDialogPopupModal(
		ImguiFileSystemDialog::OPEN_FILE_TITLE,
		getEditorUI());

	if(fsDialog.dialogClosed())
	{
		if(!fsDialog.getSelectedItem().isEmpty())
		{
			Path imageFile = fsDialog.getSelectedTarget();
			addImage(imageFile.toAbsoluteString(), imageFile);
		}
	}
}

void ImguiEditorImageViewer::buildBottomToolbar()
{
	const ImGuiStyle& style = ImGui::GetStyle();
	ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();

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

	if(hasSelectedImage())
	{
		ImageState& state = getSelectedImageState();
		size = state.actualSize;
		format = state.textureFormat;
		scale = state.actualSize.x() > 0 ? state.sizeInWindow.x() / state.actualSize.x() : 1;
		pointedPixelPos = state.pointedPixelPos;
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

void ImguiEditorImageViewer::applyZoomTo(
	ImageState& state,
	const float zoomSteps,
	const math::Vector2F& zoomCenterInWindow)
{
	if(state.sizeInWindow.x() <= 0 || state.sizeInWindow.y() <= 0)
	{
		return;
	}

	const auto sensitivity = getEditorUI().getEditor().getSettings().imageZoomSensitivity;
	const auto additionalScale = 0.05f * zoomSteps * sensitivity;
	const auto additionalSize = state.sizeInWindow * additionalScale;

	// Distribute additional scale proportionally to the offset to min/max vertices, so the scaling
	// process will appear to be centered around the specified zoom center (e.g., mouse cursor)
	state.minPosInWindow -= 
		(zoomCenterInWindow - state.minPosInWindow) / state.sizeInWindow * additionalSize;
	state.sizeInWindow += additionalSize;
}

void ImguiEditorImageViewer::setCurrentImage(std::string_view name)
{
	const auto stateIdx = getImageStateIndex(name);
	if(stateIdx < m_imageStates.size())
	{
		m_currentImageIdx = stateIdx;
	}
}

void ImguiEditorImageViewer::addImage(std::string_view name, const Path& imageFile)
{
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	if(!imageFile.isEmpty() && !imageLib.has(name))
	{
		imageLib.loadImage(name, imageFile);
		m_imageStates.push_back({
			.name = std::string(name)});

		// Make the added image current automatically. This is the typical behavior user would expect.
		setCurrentImage(name);
	}
	else
	{
		PH_DEFAULT_LOG_WARNING(
			"Failed to add image (file={}) for display. Name collision={}.",
			imageFile.toAbsoluteString(), imageLib.has(name));
	}
}

void ImguiEditorImageViewer::addImage(
	std::string_view name,
	math::Vector2UI sizePx,
	ghi::ESizedPixelFormat format)
{
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	if(sizePx.product() > 0 && !imageLib.has(name))
	{
		imageLib.loadImage(name, sizePx, format);
		m_imageStates.push_back({
			.name = std::string(name)});

		// Make the added image current automatically. This is the typical behavior user would expect.
		setCurrentImage(name);
	}
	else
	{
		PH_DEFAULT_LOG_WARNING(
			"Failed to add image buffer (size={}) for display. Name collision={}.",
			sizePx, imageLib.has(name));
	}
}

void ImguiEditorImageViewer::removeImage(std::string_view name)
{
	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();

	imageLib.unloadImage(name);

	// Note: erase at the end as `name` may depend on the string provided by image state
	const auto numErased = std::erase_if(m_imageStates,
		[name](const ImageState& state)
		{
			return state.name == name;
		});
	PH_ASSERT_LE(numErased, 1);

	// Next valid image may not automatically be current if the removed image is the last one
	if(numErased == 1 && !m_imageStates.empty() && m_currentImageIdx == m_imageStates.size())
	{
		PH_ASSERT_GE(m_currentImageIdx, 1);
		--m_currentImageIdx;
	}
}

void ImguiEditorImageViewer::setImagePixelIndicators(
	std::string_view name,
	TSpanView<math::TAABB2D<int32>> rectIndicators)
{
	const auto stateIdx = getImageStateIndex(name);
	if(stateIdx >= m_imageStates.size())
	{
		return;
	}

	m_imageStates[stateIdx].rectPixelIndicators.assign(rectIndicators.begin(), rectIndicators.end());
}

bool ImguiEditorImageViewer::hasImage(std::string_view name) const
{
	return getEditorUI().getImageLibrary().has(name);
}

math::Vector2UI ImguiEditorImageViewer::getImageSizePx(std::string_view name) const
{
	return getEditorUI().getImageLibrary().getSizePx(name);
}

render::TextureHandle ImguiEditorImageViewer::getImageHandle(std::string_view name) const
{
	return getEditorUI().getImageLibrary().getHandle(name);
}

ghi::TextureHandle ImguiEditorImageViewer::getImageGraphicsHandle(std::string_view name) const
{
	return getEditorUI().getImageLibrary().getGraphicsHandle(name);
}

}// end namespace ph::editor
