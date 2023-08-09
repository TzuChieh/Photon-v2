#pragma once

#include "EditorCore/TEventListener.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"
#include "Designer/Event/DesignerObjectAddedEvent.h"
#include "Designer/Event/DesignerObjectRemovalEvent.h"

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
	enum class ESortMode
	{
		None = 0,
		AscendingName,
		DescendingName,
		AscendingType,
		DescendingType
	};

	void onActiveSceneChanged(const ActiveDesignerSceneChangedEvent& e);
	void onSceneObjectChanged(const DesignerSceneEvent& e);
	void resetObjectView(DesignerScene* scene);
	void rebuildObjectView(DesignerScene* scene, ESortMode sortMode);

	void buildObjectsContent(DesignerScene* scene);
	void buildFiltersContent(DesignerScene& scene);
	void buildStatsContent(DesignerScene& scene);
	void buildVisibilityToggle(DesignerObject& obj);

	bool isViewingRootLevel() const;

	TEventListener<ActiveDesignerSceneChangedEvent>* m_activeSceneChanged;
	TEventListener<DesignerObjectAddedEvent>* m_sceneObjectAdded;
	TEventListener<DesignerObjectRemovalEvent>* m_sceneObjectRemoval;
	std::size_t m_objViewLevel;
	std::string m_objViewLevelName;
	DesignerObject* m_expandedObj;
	std::vector<DesignerObject*> m_objs;
	bool m_isObjsDirty;
};

inline bool ImguiEditorSceneObjectBrowser::isViewingRootLevel() const
{
	return m_objViewLevel == 0;
}

}// end namespace ph::editor
