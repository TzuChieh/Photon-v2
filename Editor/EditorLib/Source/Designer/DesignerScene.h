#pragma once

#include "Designer/designer_fwd.h"
#include "Designer/ViewportCamera.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/TSpan.h>
#include <Utility/INoCopyAndMove.h>
#include <SDL/SceneDescription.h>

#include <vector>
#include <memory>
#include <cstddef>
#include <string>

namespace ph::editor
{

class Editor;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

PH_DECLARE_LOG_GROUP(DesignerScene);

class DesignerScene final : private INoCopyAndMove
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

	template<typename ObjectType, typename... DeducedArgs>
	std::shared_ptr<ObjectType> initNewSharedRootObject(DeducedArgs&&... args);

	void deleteObject(DesignerObject* obj);
	void renderCleanup(RenderThreadCaller& caller);
	void cleanup();

	Editor& getEditor();
	const Editor& getEditor() const;
	const std::string& getName() const;
	SceneDescription& getRenderDescription();
	const SceneDescription& getRenderDescription() const;
	TSpanView<DesignerObject*> getRootObjects() const;
	void pause();
	void resume();
	bool isPaused() const;

	/*! @brief Find objects of a matching type.
	@tparam ObjectType Type of the objects to be found. Only types in the hierarchy of `DesignerObject`
	is allowed, including all bases of `DesignerObject`.
	*/
	template<typename ObjectType>
	void findObjectsByType(std::vector<ObjectType*>& out_objs) const;

	void setName(std::string name);

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

	template<typename ObjectType, typename... DeducedArgs>
	ObjectType* makeObjectFromStorage(DeducedArgs&&... args);

	bool removeObjectFromStorage(DesignerObject* obj);

private:
	TUniquePtrVector<DesignerObject> m_objStorage;
	std::vector<uint64> m_freeObjStorageIndices;
	std::vector<DesignerObject*> m_rootObjs;
	std::vector<DesignerObject*> m_tickingObjs;
	std::vector<DesignerObject*> m_renderTickingObjs;
	std::vector<ObjectAction> m_objActionQueue;
	std::size_t m_numObjActionsToProcess;

	Editor* m_editor;
	std::string m_name;
	SceneDescription m_renderDescription;
	ViewportCamera m_mainCamera;

	uint32 m_isPaused : 1;
};

}// end namespace ph::editor

#include "Designer/DesignerScene.ipp"
