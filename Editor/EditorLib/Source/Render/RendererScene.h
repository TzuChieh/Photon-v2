#pragma once

#include "Render/CustomRenderContent.h"
#include "Render/RendererResource.h"
#include "Render/View/ProjectiveView.h"

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
class RendererScene final : private IMoveOnly
{
public:
	RendererScene();
	RendererScene(RendererScene&& other);
	~RendererScene();

	void addResource(std::unique_ptr<RendererResource> resource);
	void setupGHIForPendingResources(GHIThreadCaller& caller);
	void cleanupGHIForPendingResources(GHIThreadCaller& caller);
	void destroyPendingResources();
	void removeResource(RendererResource* resourcePtr);

	void addCustomRenderContent(std::unique_ptr<CustomRenderContent> content);
	void updateCustomRenderContents(const RenderThreadUpdateContext& ctx);
	void createGHICommandsForCustomRenderContents(GHIThreadCaller& caller);
	void removeCustomRenderContent(CustomRenderContent* content);

	void reportResourceStates();

	RendererScene& operator = (RendererScene&& rhs);

public:
	ProjectiveView mainView;

private:
	TUniquePtrVector<RendererResource> m_resources;
	std::vector<RendererResource*> m_resourcesPendingSetup;
	std::vector<RendererResource*> m_resourcesPendingCleanup;
	std::vector<RendererResource*> m_resourcesPendingDestroy;
	std::vector<CustomRenderContent*> m_customRenderContents;
};

}// end namespace ph::editor
