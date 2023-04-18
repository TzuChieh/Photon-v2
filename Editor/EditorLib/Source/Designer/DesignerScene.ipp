#pragma once

#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "EditorCore/Thread/Threads.h"

#include <Utility/traits.h>
#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::initNewRootObject(DeducedArgs&&... args)
{
	PH_ASSERT(Threads::isOnMainThread());

	ObjectType* obj = initNewObject<ObjectType>(std::forward<DeducedArgs>(args)...);
	obj->getState().turnOn({EObjectState::Root});
	m_rootObjs.push_back(obj);
	return obj;
}

template<typename ObjectType, typename... DeducedArgs>
inline ObjectType* DesignerScene::initNewObject(DeducedArgs&&... args)
{
	static_assert(CDerived<ObjectType, DesignerObject>,
		"Object type must be a designer object.");

	PH_ASSERT(Threads::isOnMainThread());

	ObjectType* obj = m_objs.add(std::make_unique<ObjectType>(std::forward<DeducedArgs>(args)...));
	PH_ASSERT(obj != nullptr);
	obj->init();
	obj->getState().turnOn({EObjectState::Initialized});

	queueObjectAction(obj, EObjectAction::Create);

	return obj;
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

}// end namespace ph::editor
