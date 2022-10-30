#pragma once

#include "RenderCore/CustomRenderContent.h"

#include <Utility/TUniquePtrVector.h>

#include <memory>
#include <vector>

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;

class RTRScene final
{
public:
	~RTRScene();

private:
	friend class RenderThread;

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands(GHIThreadCaller& caller);

private:
	TUniquePtrVector<CustomRenderContent> customRenderContents;
};

}// end namespace ph::editor
