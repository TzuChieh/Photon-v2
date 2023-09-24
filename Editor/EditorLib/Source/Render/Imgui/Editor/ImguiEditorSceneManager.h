#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

#include <cstddef>

namespace ph::editor
{

class ImguiEditorSceneManager : public ImguiEditorPanel
{
public:
	explicit ImguiEditorSceneManager(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	std::size_t m_selectedSceneIdx;
};

}// end namespace ph::editor
