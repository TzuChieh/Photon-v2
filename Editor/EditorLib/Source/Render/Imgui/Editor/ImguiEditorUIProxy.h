#pragma once

namespace ph::editor
{

class ImguiEditorUI;
class Editor;
class ImguiFontLibrary;
class ImguiImageLibrary;
class ImguiFileSystemDialog;

class ImguiEditorUIProxy final
{
	// Implementation must make sure to use forward declaration as much as possible. This class
	// referenced many foundamental types across several type categories; care must be taken to
	// not introduce cyclic references.
public:
	ImguiEditorUIProxy(ImguiEditorUI& editorUI);

	Editor& getEditor();
	ImguiFontLibrary& getFontLibrary();
	ImguiImageLibrary& getImageLibrary();
	ImguiFileSystemDialog& getGeneralFileSystemDialog();

private:
	ImguiEditorUI& m_editorUI;
};

}// end namespace ph::editor
