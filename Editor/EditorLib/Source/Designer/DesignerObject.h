#pragma once

#include "Designer/AbstractDesignerObject.h"
#include "Designer/designer_fwd.h"

#include <Common/primitive_type.h>
#include <Utility/TSpan.h>
#include <SDL/sdl_interface.h>
#include <Math/math_fwd.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <Utility/TFunction.h>

#include <string>
#include <memory>
#include <cstddef>
#include <vector>
#include <type_traits>

namespace ph::editor::render { class System; }

namespace ph::editor
{

class RenderThreadCaller;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;
class RenderData;

class DesignerObject : public AbstractDesignerObject
{
	using Base = AbstractDesignerObject;

public:
	// Work type should be kept in sync with `RenderThread::Work`
	using RenderWorkType = TFunction<void(render::System&)>;

	/*!
	@return A name that is unique within the current scene. The name is also unique if the scene
	is loaded.
	*/
	static std::string generateObjectName();

public:
	~DesignerObject() override;

	virtual TSpanView<DesignerObject*> getChildren() const = 0;
	virtual bool canHaveChildren() const = 0;
	virtual math::TDecomposedTransform<real> getLocalToParent() const = 0;
	virtual void setLocalToParent(const math::TDecomposedTransform<real>& transform) = 0;

	virtual void init();
	virtual void uninit();
	virtual void renderInit(RenderThreadCaller& caller);
	virtual void renderUninit(RenderThreadCaller& caller);
	virtual void update(const MainThreadUpdateContext& ctx);
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	virtual void createRenderCommands(RenderThreadCaller& caller);

	virtual void editorTranslate(const math::Vector3R& amount);
	virtual void editorRotate(const math::QuaternionR& additionalRotation);
	virtual void editorScale(const math::Vector3R& amount);

	UIPropertyLayout layoutProperties() override;

	/*! @brief Create and add the new object as a child.
	@param shouldInit Whether to initialize the new child.
	@param shouldSetToDefault Whether to set the new child to default as defined by its SDL interface.
	*/
	template<typename ChildType>
	ChildType* newChild(
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Similar to its templated version, except that child type is inferred dynamically.
	*/
	DesignerObject* newChild(
		const SdlClass* clazz,
		bool shouldInit = true,
		bool shouldSetToDefault = true);

	/*! @brief Add the object as a child. 
	This method allows an object to be created separately then added to this object as a child later.
	*/
	DesignerObject* addNewChild(DesignerObject* childObj);

	/*! @brief Remove, uninitialize and destruct a child.
	*/
	void deleteChild(DesignerObject* childObj);

	/*! @brief Remove, uninitialize and destruct all children.
	*/
	void deleteAllChildren();

	/*! @brief Add a render thread work for this object.
	This method is intended for piecemeal (better to be independent & atomic) render data update.
	If the object is already ticking or such render data update is systematic/periodic, consider
	updating via `createRenderCommands()` or other means as they can have slightly lower overhead.
	*/
	void enqueueRenderWork(RenderWorkType work);

	/*! @brief Set name of the object.
	`name` should be unique in the scene. If not, the name will be modified automatically such that
	it is unique in the scene.
	*/
	void setName(std::string name);

	/*! @brief Set a unique name to the object.
	@param uniqueName A name that is unique in the scene. If the uniqueness cannot be guaranteed by
	the caller, use `setName()` instead.
	*/
	void setUniqueName(std::string uniqueName);

	void select();
	void deselect();
	bool isSelected() const;
	void setVisibility(bool isVisible);
	bool isVisible() const;
	void setTick(bool shouldTick);
	void setRenderTick(bool shouldTick);
	bool haveChildren() const;
	DesignerScene& getScene();
	const DesignerScene& getScene() const;
	DesignerObject* getParent();
	const DesignerObject* getParent() const;
	const std::string& getName() const;

protected:
	DesignerObject();
	DesignerObject(const DesignerObject& other);
	DesignerObject(DesignerObject&& other) noexcept;

	DesignerObject& operator = (const DesignerObject& rhs);
	DesignerObject& operator = (DesignerObject&& rhs) noexcept;

private:
	// For accessing `setParentScene()` when creating root objects
	friend class DesignerScene;

	/*! @brief Called when a child is initialized and is ready to be attached to their parent.
	@return The child that had just been added.
	*/
	virtual DesignerObject* addChild(DesignerObject* childObj) = 0;

	/*! @brief Called when a child is going to be deleted and should be removed from their parent.
	@return Whether the child had actually been removed.
	*/
	virtual bool removeChild(DesignerObject* childObj) = 0;

	/*! @brief Called when this object is selected.
	*/
	virtual void selected();

	/*! @brief Called when this object is deselected.
	*/
	virtual void deselected();

	void setParentObject(DesignerObject* object);
	void setParentScene(DesignerScene* scene);

private:
	union GeneralParent
	{
		DesignerScene* u_scene;
		DesignerObject* u_object;
	};
	
	GeneralParent m_parent;

	// SDL-binded fields:
	std::string m_name;

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
		name.noDefault();// we are supplying custom default name in ctor
		clazz.addField(name);

		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/DesignerObject.ipp"
