#pragma once

#include "Render/CustomContent.h"
#include "Render/SceneResource.h"
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
namespace ph::editor { class GHIThreadCaller; }

namespace ph::editor::render
{

class UpdateContext;
class System;

/*! @brief A scene for the editor renderer only.
*/
class Scene final : private INoCopyAndMove
{
public:
	explicit Scene(System& sys);
	~Scene();

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	TextureHandle declareTexture();

	void createTexture(TextureHandle handle, Texture texture);
	void removeTexture(TextureHandle handle);
	void loadPicture(TextureHandle handle, const Path& pictureFile);

	void addResource(std::unique_ptr<SceneResource> resource);
	void setupGHIForPendingResources(GHIThreadCaller& caller);
	void cleanupGHIForPendingResources(GHIThreadCaller& caller);
	void destroyPendingResources();
	void removeResource(SceneResource* resourcePtr);

	void addCustomRenderContent(std::unique_ptr<CustomContent> content);
	void updateCustomRenderContents(const UpdateContext& ctx);
	void createGHICommandsForCustomRenderContents(GHIThreadCaller& caller);
	void removeCustomRenderContent(CustomContent* content);

	void reportResourceStates();

public:
	ProjectiveView mainView;

private:
	System& m_sys;

	template<typename Content, CWeakHandle Handle>
	using TContentPool = TItemPool<Content, TConcurrentHandleDispatcher<Handle>>;

	TContentPool<Texture, TextureHandle> m_textures;

	TUniquePtrVector<SceneResource> m_resources;
	std::vector<SceneResource*> m_resourcesPendingSetup;
	std::vector<SceneResource*> m_resourcesPendingCleanup;
	std::vector<SceneResource*> m_resourcesPendingDestroy;
	std::vector<CustomContent*> m_customRenderContents;
};

}// end namespace ph::editor::render
