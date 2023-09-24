#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "Designer/UI/UIPropertyLayout.h"

namespace ph::editor
{

class ImguiEditorPropertyPanel : public ImguiEditorPanel
{
public:
	explicit ImguiEditorPropertyPanel(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

	void setLayout(UIPropertyLayout layout);
	void clearLayout();

private:
	UIPropertyLayout m_layout;
};

}// end namespace ph::editor
