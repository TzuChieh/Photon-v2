#pragma once

#include "RenderCore/CustomRenderContent.h"
#include "RenderCore/RTRResource.h"

#include <Utility/TUniquePtrVector.h>

#include <memory>

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;
class GHITexture2D;
class GHIFramebuffer;

/*! @brief A real-time scene for the graphics device only.
*/
class RTRScene final
{
public:
	~RTRScene();

	void addCustomRenderContent(std::unique_ptr<CustomRenderContent> content);
	void removeCustomRenderContent(CustomRenderContent* content);

private:
	friend class RenderThread;

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands(GHIThreadCaller& caller);

private:
	TUniquePtrVector<CustomRenderContent> m_customRenderContents;
	TUniquePtrVector<RTRResource>
};

}// end namespace ph::editor
