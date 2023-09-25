#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "RenderCore/ghi_infos.h"

#include "ThirdParty/DearImGui.h"

#include <Math/TVector2.h>

#include <cstddef>
#include <string>
#include <vector>

namespace ph::editor
{

class ImguiEditorImageViewer : public ImguiEditorPanel
{
public:
	explicit ImguiEditorImageViewer(ImguiEditorUIProxy editorUI);
	~ImguiEditorImageViewer();

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

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
	};

	void buildTopToolbar();
	void buildBottomToolbar();
	void pushToolbarStyleAndColor();
	void popToolbarStyleAndColor();
	bool hasSelectedImage() const;

	std::vector<ImageState> m_imageStates;
	std::size_t m_currentImageIdx;
	std::string m_imageInfoBuffer;
	math::Vector2F m_lastMouseDragDelta;
	math::Vector2F m_viewAreaMin;
	math::Vector2F m_viewAreaSize;
};

inline bool ImguiEditorImageViewer::hasSelectedImage() const
{
	return m_currentImageIdx < m_imageStates.size();
}

}// end namespace ph::editor
