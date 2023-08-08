#pragma once

#include "EditorCore/TEventListener.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>
#include <string>
#include <string_view>

namespace ph::editor
{

class ImguiEditorUIProxy;
class DesignerScene;
class DesignerObject;
class ActiveDesignerSceneChangedEvent;

class ImguiEditorSceneObjectBrowser final
{
public:
	ImguiEditorSceneObjectBrowser();

	void initialize(ImguiEditorUIProxy editorUI);
	void terminate(ImguiEditorUIProxy editorUI);

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	void onActiveSceneChanged(const ActiveDesignerSceneChangedEvent& e);
	void resetObjectViewLevels(DesignerScene* scene);

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

	TEventListener<ActiveDesignerSceneChangedEvent>* m_activeSceneChanged;
	DesignerObject* m_currentObj;
	std::size_t m_objViewLevel;
	std::string m_objViewLevelName;
	std::vector<char> m_displayNameBuffer;
};

}// end namespace ph::editor
