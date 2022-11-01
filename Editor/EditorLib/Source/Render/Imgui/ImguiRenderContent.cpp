#include "Render/Imgui/ImguiRenderContent.h"
#include "RenderCore/RenderThreadUpdateContext.h"

namespace ph::editor
{

ImguiRenderContent::ImguiRenderContent()
	: CustomRenderContent(ERenderTiming::AfterMainScene)
	, m_imguiDrawDataBuffer()
	, m_imguiDrawListBuffer()
	, m_imguiDrawListPtrBuffer()
{}

void ImguiRenderContent::copyNewDrawDataFromMainThread(
	const ImDrawData& imguiDrawData, 
	const std::size_t frameCycleIndex)
{
	ImDrawData& dstImguiDrawData = m_imguiDrawDataBuffer[frameCycleIndex];
	TUniquePtrVector<ImDrawList>& dstimgui


	// TODO
}

void ImguiRenderContent::ImguiRenderData::copyFrom(const ImDrawData& imguiDrawData)
{
	// TODO
}

}// end namespace ph::editor
