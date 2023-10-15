#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "RenderCore/ghi_infos.h"
#include "Render/Content/fwd.h"

#include "ThirdParty/DearImGui.h"

#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Math/TVector2.h>
#include <Math/Geometry/TAABB2D.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ph { class Path; }

namespace ph::editor
{

class ImguiEditorImageViewer : public ImguiEditorPanel
{
public:
	explicit ImguiEditorImageViewer(ImguiEditorUIProxy editorUI);
	~ImguiEditorImageViewer();

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

	/*! @brief Set currently displayed image by name.
	Nothing is done if the image is not found.
	*/
	void setCurrentImage(std::string_view name);

	/*! @brief Add an image file to display.
	*/
	void addImage(std::string_view name, const Path& imageFile);

	/*! @brief Add an empty image buffer for display.
	*/
	void addImage(
		std::string_view name, 
		math::Vector2UI sizePx,
		ghi::ESizedPixelFormat format = ghi::ESizedPixelFormat::RGB_8);

	/*! @brief Remove an image by name.
	Nothing is done if the image is not found.
	*/
	void removeImage(std::string_view name);

	void setImagePixelIndicators(
		std::string_view name,
		TSpanView<math::TAABB2D<int32>> rectIndicators);

	bool hasImage(std::string_view name) const;

	/*! @brief Get the size of an image.
	@return Size of the image in pixels. Will be (0, 0) if the image is not ready yet or no image
	named `name`.
	*/
	math::Vector2UI getImageSizePx(std::string_view name) const;

	/*! @brief Get the texture handle of an image.
	The handle may not be readily available and might change. To ensure freshness, always obtain
	handle from this method and do not cache it. When using a cached handle, always assume it can
	be stale or empty to ensure correctness.
	*/
	render::TextureHandle getImageHandle(std::string_view name) const;

	/*! @brief Get the graphics handle of an image.
	The availability of the handle is similar to `getImageHandle()`. Graphics handle is often useful
	for lower level manipulation of the image, whereas render handle is more convenient with scene
	resource level management.
	*/
	ghi::TextureHandle getImageGraphicsHandle(std::string_view name) const;

private:
	struct ImageState
	{
		std::string name;
		ImTextureID textureID = nullptr;
		ghi::ESizedPixelFormat textureFormat = ghi::ESizedPixelFormat::Empty;
		math::Vector2F actualSize = {0, 0};
		math::Vector2F minPosInWindow = {0, 0};
		math::Vector2F sizeInWindow = {0, 0};
		math::Vector2F pointedPixelPos = {0, 0};
		std::vector<math::TAABB2D<int32>> rectPixelIndicators;
	};

	void buildTopToolbar();
	void buildBottomToolbar();
	void pushToolbarStyleAndColor();
	void popToolbarStyleAndColor();
	bool hasSelectedImage() const;
	auto getSelectedImageState() -> ImageState&;
	std::size_t getImageStateIndex(std::string_view name) const;

	void applyZoomTo(
		ImageState& state, 
		float zoomSteps, 
		const math::Vector2F& zoomCenterInWindow);

	std::vector<ImageState> m_imageStates;
	std::size_t m_currentImageIdx;
	std::string m_imageInfoBuffer;
	math::Vector2F m_lastMouseDragDelta;
	math::Vector2F m_viewAreaMin;
	math::Vector2F m_viewAreaSize;
	bool m_showIndicators;
};

inline bool ImguiEditorImageViewer::hasSelectedImage() const
{
	return m_currentImageIdx < m_imageStates.size();
}

inline auto ImguiEditorImageViewer::getSelectedImageState()
-> ImageState&
{
	PH_ASSERT_LT(m_currentImageIdx, m_imageStates.size());
	return m_imageStates[m_currentImageIdx];
}

inline std::size_t ImguiEditorImageViewer::getImageStateIndex(std::string_view name) const
{
	for(std::size_t i = 0; i < m_imageStates.size(); ++i)
	{
		if(m_imageStates[i].name == name)
		{
			return i;
		}
	}
	return static_cast<std::size_t>(-1);
}

}// end namespace ph::editor
