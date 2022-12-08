#pragma once

#include "Render/RTRResource.h"

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;

enum class ERenderTiming
{
	BeforeMainScene,
	AfterMainScene
};

class CustomRenderContent : public RTRResource
{
public:
	explicit CustomRenderContent(ERenderTiming renderTiming);
	~CustomRenderContent() override;

	virtual void update(const RenderThreadUpdateContext& ctx) = 0;
	virtual void createGHICommands(GHIThreadCaller& caller) = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;

	ERenderTiming getRenderTiming() const;

private:
	ERenderTiming m_renderTiming;
};

inline ERenderTiming CustomRenderContent::getRenderTiming() const
{
	return m_renderTiming;
}

}// end namespace ph::editor
