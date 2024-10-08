#pragma once

#include "EditorCore/TEventDispatcher.h"

#include <utility>
#include <type_traits>

namespace ph::editor
{

template<typename EventType, typename ClassType>
class TClassEventDispatcher final
{
private:
	static_assert(std::is_class_v<ClassType>);

	using DispatcherType = TEventDispatcher<EventType>;

public:
	using Listener = DispatcherType::Listener;

	/*! @brief Same as `TEventDispatcher::addListener()`.
	*/
	Listener* addListener(Listener listener);

	template<auto MethodPtr, typename Class>
	Listener* addListener(Class* instancePtr);

	/*! @brief Same as `TEventDispatcher::removeListener()`.
	*/
	void removeListener(Listener* listener);

	/*! @brief Same as `TEventDispatcher::removeListenerImmediately()`.
	*/
	void removeListenerImmediately(Listener* listener);

private:
	friend ClassType;

	/*! @brief Same as `TEventDispatcher::dispatch()`.
	Hidden as for some implementation (such as `Editor`), event may be dispatched with extra logic.
	This is to prevent calling the raw/original dispatch accidentally.
	*/
	template<typename DispatchFunc>
	void dispatch(const EventType& e, DispatchFunc dispatchFunc);

	void dispatch(const EventType& e);

	DispatcherType m_dispatcher;
};

template<typename EventType, typename ClassType>
inline auto TClassEventDispatcher<EventType, ClassType>::addListener(
	Listener listener)
-> Listener*
{
	return m_dispatcher.addListener(std::move(listener));
}

template<typename EventType, typename ClassType>
template<auto MethodPtr, typename Class>
inline auto TClassEventDispatcher<EventType, ClassType>::addListener(
	Class* const instancePtr)
-> Listener*
{
	return m_dispatcher.addListener(Listener{}.template set<MethodPtr>(instancePtr));
}

template<typename EventType, typename ClassType>
inline void TClassEventDispatcher<EventType, ClassType>::removeListener(
	Listener* const listener)
{
	m_dispatcher.removeListener(listener);
}

template<typename EventType, typename ClassType>
inline void TClassEventDispatcher<EventType, ClassType>::removeListenerImmediately(
	Listener* const listener)
{
	m_dispatcher.removeListenerImmediately(listener);
}

template<typename EventType, typename ClassType>
template<typename DispatchFunc>
inline void TClassEventDispatcher<EventType, ClassType>::dispatch(
	const EventType& e, DispatchFunc dispatchFunc)
{
	m_dispatcher.dispatch(e, std::move(dispatchFunc));
}

template<typename EventType, typename ClassType>
inline void TClassEventDispatcher<EventType, ClassType>::dispatch(
	const EventType& e)
{
	m_dispatcher.dispatch(e);
}

}// end namespace ph
