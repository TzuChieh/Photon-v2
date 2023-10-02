#pragma once

#include "EditorCore/TEventListener.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"
#include "Designer/Event/DesignerObjectAddedEvent.h"
#include "Designer/Event/DesignerObjectRemovalEvent.h"
#include "Render/Imgui/ImguiEditorPanel.h"

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

class ImguiEditorSceneObjectBrowser : public ImguiEditorPanel
{
public:
	explicit ImguiEditorSceneObjectBrowser(ImguiEditorUIProxy editorUI);
	~ImguiEditorSceneObjectBrowser();

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	enum class ESortMode
	{
		None = 0,
		AscendingName,
		DescendingName,
		AscendingType,
		DescendingType
	};

	struct ObjectInfo
	{
		DesignerObject* obj = nullptr;
		std::string_view typeName;
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
	std::string_view getDisplayTypeName(const DesignerObject* obj) const;

	TEventListener<ActiveDesignerSceneChangedEvent>* m_activeSceneChanged;
	TEventListener<DesignerObjectAddedEvent>* m_sceneObjectAdded;
	TEventListener<DesignerObjectRemovalEvent>* m_sceneObjectRemoval;
	std::size_t m_objViewLevel;
	std::string m_objViewLevelName;
	DesignerObject* m_expandedObj;
	std::vector<ObjectInfo> m_objInfos;
	bool m_isObjsDirty;
	bool m_useDescTypeForObjType;
};

inline bool ImguiEditorSceneObjectBrowser::isViewingRootLevel() const
{
	return m_objViewLevel == 0;
}

}// end namespace ph::editor
