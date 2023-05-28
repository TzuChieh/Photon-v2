#pragma once

namespace ph::editor
{

class ImguiEditorUI;
class FileSystemExplorer;
class ImguiFileSystemDialog;

class ImguiEditorUIProxy final
{
public:
	ImguiEditorUIProxy(ImguiEditorUI& editorUI);

	ImguiFileSystemDialog& getGeneralFileSystemDialog();

private:
	ImguiEditorUI& m_editorUI;
};

}// end namespace ph::editor
