#pragma once

#include "Render/CustomRenderContent.h"
#include "Render/RendererResource.h"
#include "Render/View/ProjectiveView.h"
#include "EditorCore/Storage/TItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "Render/Content/Texture.h"

#include <Utility/TUniquePtrVector.h>
#include <Utility/INoCopyAndMove.h>

#include <memory>
#include <vector>

namespace ph { class Path; }

namespace ph::editor::render { class System; }

namespace ph::editor
{

class RenderThreadUpdateContext;
class GHIThreadCaller;

/*! @brief A scene for the editor renderer only.
*/
class RendererScene final : private INoCopyAndMove
{
public:
	explicit RendererScene(render::System& sys);
	~RendererScene();

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	render::TextureHandle declareTexture();

	void createTexture(render::TextureHandle handle, render::Texture texture);
	void removeTexture(render::TextureHandle handle);
	void loadPicture(render::TextureHandle handle, const Path& pictureFile);

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

public:
	ProjectiveView mainView;

private:
	render::System& m_sys;

	template<typename Content, CWeakHandle Handle>
	using TContentPool = TItemPool<Content, TConcurrentHandleDispatcher<Handle>>;

	TContentPool<render::Texture, render::TextureHandle> m_textures;

	TUniquePtrVector<RendererResource> m_resources;
	std::vector<RendererResource*> m_resourcesPendingSetup;
	std::vector<RendererResource*> m_resourcesPendingCleanup;
	std::vector<RendererResource*> m_resourcesPendingDestroy;
	std::vector<CustomRenderContent*> m_customRenderContents;
};

}// end namespace ph::editor
