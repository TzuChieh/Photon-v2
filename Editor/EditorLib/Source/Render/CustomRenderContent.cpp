#include "Render/CustomRenderContent.h"

namespace ph::editor
{

CustomRenderContent::CustomRenderContent(const ERenderTiming renderTiming)
	: m_renderTiming(renderTiming)
{}

CustomRenderContent::~CustomRenderContent() = default;

}// end namespace ph::editor
