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
	void removeResource(RTRResource* resourcePtr);
	void addCustomRenderContent(std::unique_ptr<CustomRenderContent> content);
	void removeCustomRenderContent(CustomRenderContent* content);

	RTRScene& operator = (RTRScene&& rhs);

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
