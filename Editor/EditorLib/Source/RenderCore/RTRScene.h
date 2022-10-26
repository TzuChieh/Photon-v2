#pragma once

#include "RenderCore/RenderThreadUpdateContext.h"
#include "RenderCore/CustomRenderContent.h"

#include <memory>
#include <vector>

namespace ph::editor
{

class RTRScene final
{
public:
	~RTRScene();

private:
	friend class RenderThread;

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands();

public:
	std::vector<std::unique_ptr<CustomRenderContent>> customRenderContents;
};

}// end namespace ph::editor
