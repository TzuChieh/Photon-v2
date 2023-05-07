#pragma once

#include "Designer/AbstractDesignerObject.h"

#include <Common/primitive_type.h>
#include <Common/config.h>
#include <Utility/INoCopyAndMove.h>
#include <Math/math.h>
#include <Utility/TBitFlags.h>
#include <Utility/TSpan.h>
#include <SDL/sdl_interface.h>

#include <string>
#include <memory>
#include <cstddef>
#include <vector>

namespace ph::editor
{

class RenderThreadCaller;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

enum class EObjectState : uint32f
{
	// Lifetime management
	Initialized = math::flag_bit<uint32f, 0>(),
	RenderInitialized = math::flag_bit<uint32f, 1>(),
	RenderUninitialized = math::flag_bit<uint32f, 2>(),
	Uninitialized = math::flag_bit<uint32f, 3>(),

	// Category
	Root = math::flag_bit<uint32f, 4>(),
	Ticking = math::flag_bit<uint32f, 5>(),
	RenderTicking = math::flag_bit<uint32f, 6>()
};

class DesignerObject
	: public AbstractDesignerObject
	, private INoCopyAndMove
{
public:
	DesignerObject();
	~DesignerObject() override;

	virtual TSpanView<DesignerObject*> getChildren() const = 0;
	virtual bool canHaveChildren() const = 0;

	virtual void init();
	virtual void uninit();
	virtual void renderInit(RenderThreadCaller& caller);
	virtual void renderUninit(RenderThreadCaller& caller);
	virtual void update(const MainThreadUpdateContext& ctx);
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	virtual void createRenderCommands(RenderThreadCaller& caller);

	/*! @brief Create, initialize and add the new object as a child.
	*/
	template<typename ChildType, typename... DeducedArgs>
	ChildType* initNewChild(DeducedArgs&&... args);

	/*! @brief Remove, uninitialize and destruct a child.
	*/
	void deleteChild(DesignerObject* childObj);

	void setName(std::string name);
	void setTick(bool shouldTick);
	void setRenderTick(bool shouldTick);
	bool haveChildren() const;
	DesignerScene& getScene();
	const DesignerScene& getScene() const;
	DesignerObject* getParent();
	const DesignerObject* getParent() const;
	const std::string& getName() const;
	const TEnumFlags<EObjectState>& getState() const;

private:
	/*! @brief Called when a child is initialized and is ready to be attached to their parent.
	@return The child that had just been added.
	*/
	virtual DesignerObject* addChild(DesignerObject* childObj) = 0;

	/*! @brief Called when a child is going to be deleted and should be removed from their parent.
	@return Whether the child had actually been removed.
	*/
	virtual bool removeChild(DesignerObject* childObj) = 0;

	void setParentObject(DesignerObject* object);

private:
	union GeneralParent
	{
		DesignerScene* u_scene;
		DesignerObject* u_object;
	};
	
	GeneralParent m_parent;
	std::string m_name;

private:
	// For accessing some shared internal data
	friend class DesignerScene;

	TEnumFlags<EObjectState> m_state;
	uint64 m_sceneStorageIndex;

	TEnumFlags<EObjectState>& getState();
	uint64 getSceneStorageIndex() const;
	void setParentScene(DesignerScene* scene);
	void setSceneStorageIndex(uint64 storageIndex);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DesignerObject>)
	{
		ClassType clazz("dobj");
		clazz.docName("Designer Object");
		clazz.description("Main base class of designer object.");

		// Creation and removal should be handled by designer scene
		clazz.allowCreateFromClass(false);

		clazz.baseOn<AbstractDesignerObject>();

		TSdlString<OwnerType> name("name", &OwnerType::m_name);
		name.description("Name of the designer object.");
		clazz.addField(name);

		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/DesignerObject.ipp"
