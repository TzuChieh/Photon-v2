#pragma once

#include "EditorCore/Event/Event.h"

namespace ph::editor
{

class AppModule;

enum class EAppModuleAction
{
	// FIXME: no longer can attach & detach during app running
	/*Attach,
	Detach*/

	// TODO: possibly paused, crashed, etc
};

class AppModuleActionEvent final : public Event
{
public:
	AppModuleActionEvent(EAppModuleAction action, AppModule* targetModule);

	EAppModuleAction getAction() const;
	AppModule* getTargetModule() const;

private:
	EAppModuleAction m_action;
	AppModule*       m_targetModule;
};

inline AppModuleActionEvent::AppModuleActionEvent(const EAppModuleAction action, AppModule* const targetModule)
	: Event()
	, m_action(action)
	, m_targetModule(targetModule)
{}

inline EAppModuleAction AppModuleActionEvent::getAction() const
{
	return m_action;
}

inline AppModule* AppModuleActionEvent::getTargetModule() const
{
	return m_targetModule;
}

}// end namespace ph::editor
