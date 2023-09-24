#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

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
};

}// end namespace ph::editor
