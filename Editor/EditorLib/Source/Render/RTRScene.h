#pragma once

#include "Render/CustomRenderContent.h"
#include "Render/RTRResource.h"

#include <Utility/TUniquePtrVector.h>
#include <Utility/IMoveOnly.h>

#include <memory>
#include <vector>

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;

/*! @brief A real-time scene for the graphics device only.
*/
class RTRScene final : private IMoveOnly
{
public:
	RTRScene();
	RTRScene(RTRScene&& other);
	~RTRScene();

	void addResource(std::unique_ptr<RTRResource> resource);
	void setupGHIForPendingResources(GHIThreadCaller& caller);
	void cleanupGHIForPendingResources(GHIThreadCaller& caller);
	void destroyPendingResources();
	void removeResource(RTRResource* resourcePtr);

	void addCustomRenderContent(std::unique_ptr<CustomRenderContent> content);
	void updateCustomRenderContents(const RenderThreadUpdateContext& ctx);
	void createGHICommandsForCustomRenderContents(GHIThreadCaller& caller);
	void removeCustomRenderContent(CustomRenderContent* content);

	void reportResourceStates();

	RTRScene& operator = (RTRScene&& rhs);

private:
	TUniquePtrVector<RTRResource> m_resources;
	std::vector<RTRResource*> m_resourcesPendingSetup;
	std::vector<RTRResource*> m_resourcesPendingCleanup;
	std::vector<RTRResource*> m_resourcesPendingDestroy;
	std::vector<CustomRenderContent*> m_customRenderContents;
};

}// end namespace ph::editor
