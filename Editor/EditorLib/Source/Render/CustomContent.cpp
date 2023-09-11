#include "Render/CustomContent.h"

namespace ph::editor::render
{

CustomContent::CustomContent(ERenderTiming renderTiming)
	: m_renderTiming(renderTiming)
{}

CustomContent::~CustomContent() = default;

}// end namespace ph::editor::render
