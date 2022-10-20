#pragma once

#include "RenderCore/RenderThreadUpdateContext.h"

#include <memory>
#include <vector>

namespace ph::editor
{

class CustomRenderContent;

class RTRScene final
{
public:
	~RTRScene();

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands();

public:
	std::vector<std::unique_ptr<CustomRenderContent>> customRenderContents;
};

}// end namespace ph::editor
