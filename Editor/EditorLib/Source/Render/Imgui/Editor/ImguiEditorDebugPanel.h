#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

#include <vector>

namespace ph::editor
{

class ImguiEditorDebugPanel : public ImguiEditorPanel
{
public:
	explicit ImguiEditorDebugPanel(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	std::vector<char> m_objectNameBuffer;
};

}// end namespace ph::editor
