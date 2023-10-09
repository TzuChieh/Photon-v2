#pragma once

namespace ph::editor
{

class ImguiEditorUI;
class Editor;
class ImguiFontLibrary;
class ImguiImageLibrary;
class ImguiEditorImageViewer;
class ImguiFileSystemDialog;
class ImguiEditorObjectTypeMenu;
class ImguiEditorTheme;

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
	ImguiEditorImageViewer& getImageViewer();
	ImguiFileSystemDialog& getGeneralFileSystemDialog();
	ImguiEditorObjectTypeMenu& getObjectTypeMenu();
	const ImguiEditorTheme& getTheme();
	bool isMain() const;

private:
	ImguiEditorUI& m_editorUI;
};

}// end namespace ph::editor
