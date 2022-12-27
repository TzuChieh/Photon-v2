#include "Render/EditorDebug/EditorDebugRenderModule.h"

#include <Common/assertion.h>

namespace ph::editor
{

EditorDebugRenderModule::EditorDebugRenderModule()
	: RenderModule()
{}

void EditorDebugRenderModule::onAttach(const ModuleAttachmentInfo& info)
{
	// TODO
}

void EditorDebugRenderModule::onDetach()
{
	// TODO
}

void EditorDebugRenderModule::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// TODO
}

void EditorDebugRenderModule::createRenderCommands(RenderThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor
