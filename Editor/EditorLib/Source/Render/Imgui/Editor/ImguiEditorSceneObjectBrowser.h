#pragma once

namespace ph::editor
{

class ImguiEditorUIProxy;
class DesignerScene;

class ImguiEditorSceneObjectBrowser final
{
public:
	ImguiEditorSceneObjectBrowser();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	void buildObjectsContent(DesignerScene& scene);
	void buildFiltersContent(DesignerScene& scene);
	void buildStatsContent(DesignerScene& scene);
};

}// end namespace ph::editor
