#pragma once

#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "EditorCore/Thread/Threads.h"

#include <Utility/traits.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <SDL/sdl_helpers.h>
#include <Utility/exception.h>

#include <utility>
#include <type_traits>

namespace ph::editor
{

namespace detail
{

template<typename ObjectType>
struct TSharedObjectDeleter
{
	inline void operator () (ObjectType* const obj) const
	{
		static_assert(CDerived<ObjectType, DesignerObject>,
			"Object must be a designer object.");

		if(!obj)
		{
			return;
		}

		obj->getScene().deleteObject(obj);
	}
};

}// end namespace detail

inline const char* DesignerScene::defaultSceneName()
{
	return "untitled scene";
}

template<typename ObjectType>
inline ObjectType* DesignerScene::newObject(
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	DesignerObject* obj = newObject(
		ObjectType::getSdlClass(),
		shouldInit,
		shouldSetToDefault);

	// We know the exact type
	return static_cast<ObjectType*>(obj);
}

template<typename ObjectType>
inline ObjectType* DesignerScene::newRootObject(
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	DesignerObject* obj = newRootObject(
		ObjectType::getSdlClass(),
		shouldInit,
		shouldSetToDefault);

	// We know the exact type
	return static_cast<ObjectType*>(obj);
}

template<typename ObjectType>
inline std::shared_ptr<ObjectType> DesignerScene::newSharedRootObject(
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	std::shared_ptr<ObjectType> rootObj = newSharedRootObject(
		ObjectType::getSdlClass(),
		shouldInit, 
		shouldSetToDefault);

	return std::static_pointer_cast<ObjectType>(std::move(rootObj));
}

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::makeObjectFromStorage(DeducedArgs&&... args)
{
	static_assert(CDerived<ObjectType, DesignerObject>,
		"Object must be a designer object.");

	PH_ASSERT(Threads::isOnMainThread());

	if(isPaused())
	{
		// This is a condition that should be investigated (API misuse)
		PH_ASSERT_MSG(false, 
			"Cannot create object when paused--this may modify object storage.");
		return nullptr;
	}

	auto storageIndex = static_cast<uint64>(-1);

	// Create new storage space
	if(m_freeObjStorageIndices.empty())
	{
		m_objStorage.add<ObjectType>(nullptr);
		storageIndex = m_objStorage.size() - 1;
	}
	// Use existing storage space
	else
	{
		storageIndex = m_freeObjStorageIndices.back();
		m_freeObjStorageIndices.pop_back();
	}

	auto uniqueObj = std::make_unique<ObjectType>(std::forward<DeducedArgs>(args)...);
	ObjectType* obj = uniqueObj.get();
	m_objStorage.getUniquePtr(storageIndex) = std::move(uniqueObj);

	PH_ASSERT(obj != nullptr);
	obj->setSceneStorageIndex(storageIndex);
	return obj;
}

template<typename ObjectType>
inline void DesignerScene::registerObjectType()
{
	static_assert(CDerived<ObjectType, DesignerObject>,
		"Object must be a designer object.");

	const SdlClass* const clazz = ObjectType::getSdlClass();
	if(classToObjMaker.find(clazz) != classToObjMaker.end())
	{
		PH_LOG_ERROR(DesignerScene,
			"designer object already registered ({})",
			sdl::gen_pretty_name(clazz));

		return;
	}
		
	classToObjMaker[clazz] = 
		[](DesignerScene& scene) -> DesignerObject*
		{
			if constexpr(std::is_abstract_v<ObjectType>)
			{
				PH_ASSERT_MSG(false,
					"Attempting to create object of an abstract type.");
				return nullptr;
			}
			else
			{
				return scene.makeObjectFromStorage<ObjectType>();
			}
		};
}

inline bool DesignerScene::removeObjectFromStorage(DesignerObject* const obj)
{
	if(isPaused())
	{
		// This is a condition that should be investigated (API misuse)
		PH_ASSERT_MSG(false,
			"Cannot remove object when paused--this may modify object storage.");

		return false;
	}

	if(!obj || &(obj->getScene()) != this || obj->getSceneStorageIndex() == static_cast<uint64>(-1))
	{
		return false;
	}

	const auto objIndex = obj->getSceneStorageIndex();
	PH_ASSERT(obj == m_objStorage[objIndex]);
	m_objStorage.getUniquePtr(objIndex) = nullptr;
	m_freeObjStorageIndices.push_back(objIndex);

	return true;
}

inline bool DesignerScene::isFullyInitialized(const DesignerObject& obj)
{
	return obj.getState().hasAll({EObjectState::HasInitialized, EObjectState::HasRenderInitialized}) &&
	       obj.getState().hasNone({EObjectState::HasUninitialized, EObjectState::HasRenderUninitialized});
}

inline bool DesignerScene::isInitialized(const DesignerObject& obj)
{
	return obj.getState().has(EObjectState::HasInitialized) &&
	       obj.getState().hasNo(EObjectState::HasUninitialized);
}

inline bool DesignerScene::isOrphan(const DesignerObject& obj)
{
	const bool isChild = obj.getState().hasNo(EObjectState::Root);
	const bool hasParent = obj.getParent() != nullptr;

	return isChild && !hasParent;
}

inline Editor& DesignerScene::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline const Editor& DesignerScene::getEditor() const
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline render::Scene& DesignerScene::getRendererScene()
{
	PH_ASSERT(Threads::isOnMainThread());// do not let the word "render" fool you
	PH_ASSERT_MSG(m_rendererScene,
		"Please make sure you are calling from render command generating methods.");

	return *m_rendererScene;
}

inline const render::Scene& DesignerScene::getRendererScene() const
{
	PH_ASSERT(Threads::isOnMainThread());// do not let the word "render" fool you
	PH_ASSERT_MSG(m_rendererScene,
		"Please make sure you are calling from render command generating methods.");

	return *m_rendererScene;
}

inline DesignerObject* DesignerScene::getPrimarySelectedObject() const
{
	return m_selectedObjs.empty() ? nullptr : m_selectedObjs.front();
}

inline TSpanView<DesignerObject*> DesignerScene::getSelection() const
{
	return m_selectedObjs;
}

inline bool DesignerScene::SceneAction::isDone() const
{
	return !updateTask && !renderTask;
}

inline const Path& DesignerScene::getWorkingDirectory() const
{
	return m_workingDirectory;
}

inline TSpanView<DesignerRendererBinding> DesignerScene::getRendererBindings() const
{
	return m_rendererBindings;
}

inline const std::string& DesignerScene::getName() const
{
	return m_name;
}

inline SceneDescription& DesignerScene::getRenderDescription()
{
	return m_renderDescription;
}

inline const SceneDescription& DesignerScene::getRenderDescription() const
{
	return m_renderDescription;
}

inline const ResourceIdentifier& DesignerScene::getRenderDescriptionLink() const
{
	return m_renderDescriptionLink;
}

inline void DesignerScene::setRenderDescriptionLink(ResourceIdentifier link)
{
	m_renderDescriptionLink = std::move(link);
}

inline TSpanView<DesignerObject*> DesignerScene::getRootObjects() const
{
	return m_rootObjs;
}

inline bool DesignerScene::isPaused() const
{
	return m_isPaused;
}

template<typename ObjectType>
inline ObjectType* DesignerScene::findObjectByName(std::string_view name) const
{
	ObjectType* result = nullptr;

	forEachUsableObject(
		[name, &result](DesignerObject* obj) -> bool
		{
			// Implicit cast if `ObjectType` is a base type
			if constexpr(CDerived<DesignerObject, ObjectType>)
			{
				if(obj->getName() == name)
				{
					result = obj;
					return false;
				}
			}
			// Otherwise explicit downcasting is required
			else
			{
				static_assert(CDerived<ObjectType, DesignerObject>,
					"Object must be a designer object.");

				if(obj->getName() == name)
				{
					// Directly use the result of `dynamic_cast`. This can be null if the type does
					// not match--which is fine, as the object name is unique (and we already found the
					// object) there is no point in continuing the search.
					result = dynamic_cast<ObjectType*>(obj);
					return false;
				}
			}

			return true;
		});

	return result;
}

template<typename ObjectType>
inline void DesignerScene::findObjectsByType(std::vector<ObjectType*>& out_objs) const
{
	out_objs.reserve(out_objs.size() + m_objStorage.size());

	forEachUsableObject(
		[&out_objs](DesignerObject* obj) -> bool
		{
			// Implicit cast if `ObjectType` is a base type
			if constexpr(CDerived<DesignerObject, ObjectType>)
			{
				out_objs.push_back(obj);
			}
			// Otherwise explicit downcasting is required
			else
			{
				static_assert(CDerived<ObjectType, DesignerObject>,
					"Object must be a designer object.");

				auto const derivedObj = dynamic_cast<ObjectType*>(obj);
				if(derivedObj)
				{
					out_objs.push_back(derivedObj);
				}
			}

			// Always continue
			return true;
		});
}

template<typename PerObjectOperation>
inline void DesignerScene::forEachUsableObject(PerObjectOperation op) const
{
	static_assert(std::is_invocable_r_v<bool, PerObjectOperation, DesignerObject*>);

	PH_ASSERT(Threads::isOnMainThread());

	for(auto& objRes : m_objStorage)
	{
		DesignerObject* const obj = objRes.get();

		// Skip removed object
		if(!obj)
		{
			continue;
		}

		// Skip object with incomplete initialization state (we do not care about render state here)
		if(!isInitialized(*obj))
		{
			continue;
		}

		// Skip orphans
		if(isOrphan(*obj))
		{
			continue;
		}

		const bool shouldContinue = op(obj);
		if(!shouldContinue)
		{
			break;
		}
	}
}

inline std::size_t DesignerScene::numRootObjects() const
{
	return m_rootObjs.size();
}

inline std::size_t DesignerScene::numTickingObjects() const
{
	return m_tickingObjs.size();
}

inline std::size_t DesignerScene::numRenderTickingObjects() const
{
	return m_renderTickingObjs.size();
}

inline std::size_t DesignerScene::numAllocatedObjects() const
{
	return m_objStorage.size();
}

}// end namespace ph::editor
