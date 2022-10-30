#include "Render/Imgui/ImguiRenderContent.h"
#include "RenderCore/RenderThreadUpdateContext.h"

namespace ph::editor
{

ImguiRenderContent::ImguiRenderContent()
	: CustomRenderContent(ERenderTiming::AfterMainScene)
{}

}// end namespace ph::editor
