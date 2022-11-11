#include "Render/Imgui/ImguiRenderModule.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "App/Editor.h"
#include "App/Module/MainThreadRenderUpdateContext.h"

namespace ph::editor
{

void ImguiRenderModule::onAttach(const ModuleAttachmentInfo& info)
{
	// TODO: listen to frame buffer size
	// TODO: record current frame buffer size?
}

void ImguiRenderModule::onDetach()
{
	// TODO
}

void ImguiRenderModule::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// TODO
}

}// end namespace ph::editor
