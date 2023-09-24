#include "Render/Imgui/ImguiEditorUIProxy.h"
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

ImguiFontLibrary& ImguiEditorUIProxy::getFontLibrary()
{
	return m_editorUI.getFontLibrary();
}

ImguiImageLibrary& ImguiEditorUIProxy::getImageLibrary()
{
	return m_editorUI.getImageLibrary();
}

ImguiFileSystemDialog& ImguiEditorUIProxy::getGeneralFileSystemDialog()
{
	return m_editorUI.getGeneralFileSystemDialog();
}

ImguiEditorObjectTypeMenu& ImguiEditorUIProxy::getObjectTypeMenu()
{
	return m_editorUI.getObjectTypeMenu();
}

const ImguiEditorTheme& ImguiEditorUIProxy::getTheme()
{
	return m_editorUI.getTheme();
}

bool ImguiEditorUIProxy::isMain() const
{
	return m_editorUI.isMain();
}

}// end namespace ph::editor
