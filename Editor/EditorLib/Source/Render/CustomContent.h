#pragma once

#include "Render/SceneResource.h"

namespace ph::editor::render
{

class UpdateContext;

enum class ERenderTiming
{
	BeforeMainScene,
	AfterMainScene
};

class CustomContent : public SceneResource
{
public:
	explicit CustomContent(ERenderTiming renderTiming);
	~CustomContent() override;

	virtual void update(const UpdateContext& ctx) = 0;
	virtual void createGHICommands(GHIThreadCaller& caller) = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;

	ERenderTiming getRenderTiming() const;

private:
	ERenderTiming m_renderTiming;
};

inline ERenderTiming CustomContent::getRenderTiming() const
{
	return m_renderTiming;
}

}// end namespace ph::editor::render
