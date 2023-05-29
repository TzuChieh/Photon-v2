#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "Render/Imgui/ImguiEditorUI.h"

namespace ph::editor
{

ImguiEditorUIProxy::ImguiEditorUIProxy(ImguiEditorUI& editorUI)
	: m_editorUI(editorUI)
{}

Editor& ImguiEditorUIProxy::getEditor()
{
	return m_editorUI.getEditor();
}

ImguiFileSystemDialog& ImguiEditorUIProxy::getGeneralFileSystemDialog()
{
	return m_editorUI.getGeneralFileSystemDialog();
}

}// end namespace ph::editor
