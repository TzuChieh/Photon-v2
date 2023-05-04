#pragma once

#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "EditorCore/Thread/Threads.h"

#include <Utility/traits.h>
#include <Common/assertion.h>

#include <utility>

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

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::initNewRootObject(DeducedArgs&&... args)
{
	PH_ASSERT(Threads::isOnMainThread());

	ObjectType* obj = initNewObject<ObjectType>(std::forward<DeducedArgs>(args)...);
	obj->setParentScene(this);
	m_rootObjs.push_back(obj);
	return obj;
}

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::initNewObject(DeducedArgs&&... args)
{
	ObjectType* obj = makeObjectFromStorage<ObjectType>(std::forward<DeducedArgs>(args)...);
	PH_ASSERT(obj != nullptr);
	obj->init();
	obj->getState().turnOn({EObjectState::Initialized});

	queueObjectAction(obj, EObjectAction::Create);

	return obj;
}

template<typename ObjectType, typename... DeducedArgs>
inline std::shared_ptr<ObjectType> DesignerScene::initNewSharedRootObject(DeducedArgs&&... args)
{
	return std::shared_ptr<ObjectType>(
		initNewRootObject<ObjectType>(std::forward<DeducedArgs>(args)...),
		detail::TSharedObjectDeleter<ObjectType>());
}

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::makeObjectFromStorage(DeducedArgs&&... args)
{
	static_assert(CDerived<ObjectType, DesignerObject>,
		"Object must be a designer object.");

	PH_ASSERT(Threads::isOnMainThread());

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

inline bool DesignerScene::removeObjectFromStorage(DesignerObject* const obj)
{
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

inline void DesignerScene::setName(std::string name)
{
	m_name = std::move(name);
}

inline const std::string& DesignerScene::getName() const
{
	return m_name;
}

inline TSpanView<DesignerObject*> DesignerScene::getRootObjects() const
{
	return m_rootObjs;
}

}// end namespace ph::editor
