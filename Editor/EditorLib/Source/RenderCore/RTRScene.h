#pragma once

#include "RenderCore/CustomRenderContent.h"
#include "RenderCore/RTRResource.h"

#include <Utility/TUniquePtrVector.h>

#include <memory>
#include <vector>

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

	void addResource(std::unique_ptr<RTRResource> resource);
	void removeResource(RTRResource* resource);
	void addCustomRenderContent(std::unique_ptr<CustomRenderContent> content);
	void removeCustomRenderContent(CustomRenderContent* content);

private:
	friend class RenderThread;

	void update(const RenderThreadUpdateContext& ctx);
	void createGHICommands(GHIThreadCaller& caller);

private:
	TUniquePtrVector<RTRResource> m_resources;
	TUniquePtrVector<RTRResource> m_resourcesPendingAdd;
	TUniquePtrVector<RTRResource> m_resourcesPendingRemove;
	TUniquePtrVector<CustomRenderContent> m_customRenderContents;
};

}// end namespace ph::editor
