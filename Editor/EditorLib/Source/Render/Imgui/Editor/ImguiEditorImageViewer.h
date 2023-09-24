#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

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
	std::vector<std::string> m_imageNames;
};

}// end namespace ph::editor
