#include "Render/Imgui/ImguiRenderModule.h"
#include "App/ModuleAttachmentInfo.h"
#include "App/Editor.h"

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

}// end namespace ph::editor
