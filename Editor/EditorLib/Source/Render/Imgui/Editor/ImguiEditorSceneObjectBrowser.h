#pragma once

#include <cstddef>
#include <vector>
#include <string_view>

namespace ph::editor
{

class ImguiEditorUIProxy;
class DesignerScene;
class DesignerObject;

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

	void buildObjectTableRowRecursive(DesignerObject& obj);
	void buildVisibilityToggle(DesignerObject& obj);

	const char* getObjectDisplayName(
		std::string_view objName, 
		std::string_view namePrefix,
		std::size_t maxChars);

	void prepareDisplayNameBuffer(std::size_t maxChars);

	std::vector<char> m_displayNameBuffer;
};

}// end namespace ph::editor
