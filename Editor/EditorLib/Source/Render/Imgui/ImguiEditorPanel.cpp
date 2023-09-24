#include "Render/Imgui/ImguiEditorPanel.h"

namespace ph::editor
{

ImguiEditorPanel::ImguiEditorPanel(ImguiEditorUIProxy editorUI)
	: m_editorUI(editorUI)
{}

ImguiEditorPanel::~ImguiEditorPanel() = default;

}// end namespace ph::editor
