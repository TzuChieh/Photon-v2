#pragma once

#include "Render/CustomRenderContent.h"
#include "Render/RendererResource.h"
#include "Render/View/ProjectiveView.h"
#include "EditorCore/Storage/TItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "Render/Content/Texture.h"

#include <Utility/TUniquePtrVector.h>
#include <Utility/IMoveOnly.h>

#include <memory>
#include <vector>

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;

/*! @brief A scene for the real-time renderer only.
*/
class RendererScene final : private IMoveOnly
{
public:
	RendererScene();
	RendererScene(RendererScene&& other);
	~RendererScene();

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	render::TextureHandle createTexture();



	void removeTexture(render::TextureHandle handle);

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
	template<typename Content, CWeakHandle Handle>
	using TContentPool = TItemPool<Content, TConcurrentHandleDispatcher<Handle>>;

	TContentPool<render::Texture, render::TextureHandle> textures;

	TUniquePtrVector<RendererResource> m_resources;
	std::vector<RendererResource*> m_resourcesPendingSetup;
	std::vector<RendererResource*> m_resourcesPendingCleanup;
	std::vector<RendererResource*> m_resourcesPendingDestroy;
	std::vector<CustomRenderContent*> m_customRenderContents;
};

}// end namespace ph::editor
