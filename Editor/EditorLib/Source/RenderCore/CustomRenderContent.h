#pragma once

#include "RenderCore/RenderThreadUpdateContext.h"

namespace ph::editor
{

enum class ERenderTiming
{
	BeforeMainScene,
	AfterMainScene
};

class CustomRenderContent
{
public:
	explicit CustomRenderContent(ERenderTiming renderTiming);
	virtual ~CustomRenderContent();

	virtual void update(const RenderThreadUpdateContext& ctx) = 0;
	virtual void createGHICommands() = 0;

	ERenderTiming getRenderTiming() const;

private:
	ERenderTiming m_renderTiming;
};

inline ERenderTiming CustomRenderContent::getRenderTiming() const
{
	return m_renderTiming;
}

}// end namespace ph::editor
