#pragma once

#include "RenderCore/CustomRenderContent.h"

namespace ph::editor
{

class ImguiRenderContent : public CustomRenderContent
{
public:
	ImguiRenderContent();

	void update(const RenderThreadUpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;

private:
};

}// end namespace ph::editor
