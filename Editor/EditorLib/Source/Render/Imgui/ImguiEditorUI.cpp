#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"

#include <Common/assertion.h>

namespace ph::editor
{

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
{}

void ImguiEditorUI::build()
{
	PH_ASSERT(Threads::isOnMainThread());
	PH_ASSERT(m_editor);

	show_imgui_demo_window();
}

void ImguiEditorUI::setEditor(Editor* const editor)
{
	PH_ASSERT(editor);

	m_editor = editor;
}

}// end namespace ph::editor
