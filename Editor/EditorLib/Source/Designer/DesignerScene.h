#pragma once

#include "Designer/designer_fwd.h"
#include "Designer/ViewportCamera.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/TSpan.h>
#include <SDL/Object.h>
#include <SDL/sdl_interface.h>
#include <SDL/SceneDescription.h>
#include <Utility/IMoveOnly.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/ResourceIdentifier.h>

#include <vector>
#include <memory>
#include <cstddef>
#include <string>
#include <functional>
#include <unordered_map>

namespace ph { class SdlClass; }

namespace ph::editor
{

class Editor;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

PH_DECLARE_LOG_GROUP(DesignerScene);

class DesignerScene final 
	: public Object
	, private IMoveOnly
{
public:
	static const char* defaultSceneName();

public:
	explicit DesignerScene(Editor* fromEditor);
	DesignerScene(DesignerScene&& other) noexcept;
	~DesignerScene() override;

	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	void createRenderCommands(RenderThreadCaller& caller);
	void beforeUpdateStage();
	void afterUpdateStage();
	void beforeRenderStage();
	void afterRenderStage();

	void markObjectTickState(DesignerObject* obj, bool markTick);
	void markObjectRenderTickState(DesignerObject* obj, bool markTick);

	template<typename ObjectType>
	[[nodiscard]]
	ObjectType* newObject(
		bool shouldInit = true, 
		bool shouldSetToDefault = true);

	template<typename ObjectType>
	ObjectType* newRootObject(
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Create root object with automatic lifetime management.
	The root object will delete itself once the `shared_ptr` free its pointer. Similar to normal objects,
	using the object pointer after the scene is removed is an error.
	*/
	template<typename ObjectType>
	std::shared_ptr<ObjectType> newSharedRootObject(
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Similar to its templated version, except that object type is inferred dynamically.
	*/
	[[nodiscard]]
	DesignerObject* newObject(
		const SdlClass* clazz,
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Similar to its templated version, except that object type is inferred dynamically.
	*/
	DesignerObject* newRootObject(
		const SdlClass* clazz,
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Similar to its templated version, except that object type is inferred dynamically.
	*/
	std::shared_ptr<DesignerObject> newSharedRootObject(
		const SdlClass* clazz,
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Initialize the object.
	By default this is called automatically when creating new object. If the object creation routine 
	has been specifically instructed to not initialize the object, then this method must be called
	manually after creating the object.
	*/
	void initObject(DesignerObject* obj);

	void setObjectToDefault(DesignerObject* obj);

	void deleteObject(DesignerObject* obj);
	void renderCleanup(RenderThreadCaller& caller);
	void cleanup();

	Editor& getEditor();
	const Editor& getEditor() const;
	const std::string& getName() const;
	SceneDescription& getRenderDescription();
	const SceneDescription& getRenderDescription() const;
	const ResourceIdentifier& getRenderDescriptionLink() const;
	void setRenderDescriptionLink(ResourceIdentifier link);
	TSpanView<DesignerObject*> getRootObjects() const;

	/*! @brief Temporarily stop the update of scene.
	When paused, most scene states should not be changed, including all contained objects. 
	Contained objects will not be updated (while render related operations are not affected).
	*/
	void pause();

	/*! @brief Continue the update of scene.
	Resuming an already-resumed scene has no effect.
	*/
	void resume();

	/*! @brief Whether the scene is pausing.
	When the scene is pausing, object and scene states should not be modified. Render states can
	ignore the pausing state.
	*/
	bool isPaused() const;

	/*! @brief Find objects of a matching type.
	@tparam ObjectType Type of the objects to be found. Only types in the hierarchy of `DesignerObject`
	is allowed, including all bases of `DesignerObject`.
	*/
	template<typename ObjectType>
	void findObjectsByType(std::vector<ObjectType*>& out_objs) const;

	const Path& getWorkingDirectory() const;
	void setWorkingDirectory(Path directory);
	void setName(std::string name);

	DesignerScene& operator = (DesignerScene&& rhs) noexcept;

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
	// Working directory is only set when it can be determined (e.g., during loading).
	// Not saved as working directory can be different on each load.
	Path m_workingDirectory;

	TUniquePtrVector<DesignerObject> m_objStorage;
	std::vector<uint64> m_freeObjStorageIndices;
	std::vector<DesignerObject*> m_rootObjs;
	std::vector<DesignerObject*> m_tickingObjs;
	std::vector<DesignerObject*> m_renderTickingObjs;
	std::vector<ObjectAction> m_objActionQueue;
	std::size_t m_numObjActionsToProcess;

	Editor* m_editor;
	SceneDescription m_renderDescription;
	ViewportCamera m_mainCamera;
	uint32 m_isPaused : 1;

	// SDL-binded fields:
	std::string m_name;
	ResourceIdentifier m_renderDescriptionLink;

/*! @brief Dynamic object creation routines	
*/
///@{
private:
	using DynamicObjectMaker = std::function<DesignerObject*(DesignerScene& scene)>;
	static std::unordered_map<const SdlClass*, DynamicObjectMaker> classToObjMaker;

public:
	template<typename ObjectType>
	static void registerObjectType();
///@}

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DesignerScene>)
	{
		ClassType clazz("dscene");
		clazz.docName("Designer Scene");
		clazz.description("Designer scene. The main container of designer objects.");

		// Creation and removal should be handled by editor
		clazz.allowCreateFromClass(false);

		clazz.baseOn<Object>();

		TSdlString<OwnerType> name("name", &OwnerType::m_name);
		name.description("Name of the designer scene.");
		name.defaultTo(defaultSceneName());
		clazz.addField(name);

		TSdlResourceIdentifier<OwnerType> renderDescriptionLink("render-description-link", &OwnerType::m_renderDescriptionLink);
		renderDescriptionLink.description("Path to the associated scene description.");
		renderDescriptionLink.defaultTo(ResourceIdentifier());
		clazz.addField(renderDescriptionLink);

		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/DesignerScene.ipp"
