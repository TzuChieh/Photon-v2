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

inline void DesignerScene::ObjectAction::done()
{
	action = EObjectAction::None;
}

inline bool DesignerScene::ObjectAction::isDone() const
{
	return action == EObjectAction::None;
}

inline const Path& DesignerScene::getWorkingDirectory() const
{
	return m_workingDirectory;
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
inline void DesignerScene::findObjectsByType(std::vector<ObjectType*>& out_objs) const
{
	out_objs.reserve(out_objs.size() + m_objStorage.size());
	for(auto& objRes : m_objStorage)
	{
		DesignerObject* const obj = objRes.get();

		// Skip removed object
		if(!obj)
		{
			continue;
		}

		// Skip object with incomplete initialization state (we do not care about render state here)
		if(obj->getState().hasNo(EObjectState::Initialized) ||
		   obj->getState().has(EObjectState::Uninitialized))
		{
			continue;
		}

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
	}
}

}// end namespace ph::editor
