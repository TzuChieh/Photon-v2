#pragma once

#include "Designer/designer_fwd.h"
#include "Designer/ViewportCamera.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/TUniquePtrVector.h>

#include <vector>
#include <memory>
#include <cstddef>

namespace ph::editor
{

class Editor;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

class DesignerScene final
{
public:
	explicit DesignerScene(Editor* fromEditor);
	~DesignerScene();

	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	void createRenderCommands(RenderThreadCaller& caller);
	void beforeUpdateStage();
	void afterUpdateStage();
	void beforeRenderStage();
	void afterRenderStage();

	void markObjectTickState(DesignerObject* obj, bool markTick);
	void markObjectRenderTickState(DesignerObject* obj, bool markTick);

	template<typename ObjectType, typename... DeducedArgs>
	[[nodiscard]]
	ObjectType* initNewObject(DeducedArgs&&... args);

	template<typename ObjectType, typename... DeducedArgs>
	ObjectType* initNewRootObject(DeducedArgs&&... args);

	void removeObject(DesignerObject* obj);

	Editor& getEditor();
	const Editor& getEditor() const;

private:
	enum class EObjectAction : uint8
	{
		None,
		Create,
		Remove,
		EnableTick,
		DisableTick,
		EnableRenderTick,
		DisableRenderTick
	};

	struct ObjectAction
	{
		DesignerObject* obj = nullptr;
		EObjectAction action = EObjectAction::None;

		void done();
		bool isDone() const;
	};

	void queueObjectAction(DesignerObject* obj, EObjectAction objAction);

private:
	TUniquePtrVector<DesignerObject> m_objs;
	std::vector<DesignerObject*> m_rootObjs;
	std::vector<DesignerObject*> m_tickingObjs;
	std::vector<DesignerObject*> m_renderTickingObjs;
	std::vector<ObjectAction> m_objActionQueue;
	std::size_t m_numObjActionsToProcess;
	Editor* m_editor;
	ViewportCamera m_mainCamera;
};

}// end namespace ph::editor

#include "Designer/DesignerScene.ipp"
