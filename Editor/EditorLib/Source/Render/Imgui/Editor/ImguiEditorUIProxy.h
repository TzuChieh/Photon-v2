#pragma once

namespace ph::editor
{

class Editor;
class ImguiEditorUI;
class ImguiFileSystemDialog;

class ImguiEditorUIProxy final
{
public:
	ImguiEditorUIProxy(ImguiEditorUI& editorUI);

	Editor& getEditor();
	ImguiFileSystemDialog& getGeneralFileSystemDialog();

private:
	ImguiEditorUI& m_editorUI;
};

}// end namespace ph::editor
