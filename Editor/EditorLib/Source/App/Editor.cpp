#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>

#include <memory>

namespace ph::editor
{

Editor::Editor() = default;

Editor::~Editor() = default;

DesignerScene& Editor::createScene(std::size_t* const out_sceneIndex)
{
	DesignerScene* const scene = m_scenes.add(std::make_unique<DesignerScene>());
	scene->setEditor(this);

	if(out_sceneIndex)
	{
		*out_sceneIndex = m_scenes.size() - 1;
	}

	return *scene;
}

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Post events to `m_eventProcessQueue`
	m_eventPostQueue.flushAllEvents();

	// Run all event process work
	for(const auto& processEventWork : m_eventProcessQueue)
	{
		processEventWork();
	}

	m_eventProcessQueue.clear();
}

}// end namespace ph::editor
