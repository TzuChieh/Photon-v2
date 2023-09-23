#pragma once

#include "Render/ISceneResource.h"
#include "Render/IDynamicSceneResource.h"
#include "Render/View/ProjectiveView.h"
#include "EditorCore/Storage/TItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "Render/Content/Texture.h"

#include <Common/assertion.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/INoCopyAndMove.h>
#include <Utility/TSortedVector.h>
#include <Utility/TFunction.h>

#include <memory>
#include <vector>
#include <string>

namespace ph { class Path; }
namespace ph::editor { class GHIThreadCaller; }
namespace ph::editor { class RenderThreadUpdateContext; }
namespace ph::editor::ghi { class TextureDesc; }

namespace ph::editor::render
{

class System;

/*! @brief A scene for the editor renderer only.
*/
class Scene final : private INoCopyAndMove
{
public:
	Scene();
	explicit Scene(std::string debugName);
	~Scene();

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	TextureHandle declareTexture();

	void createTexture(TextureHandle handle, const ghi::TextureDesc& desc);
	Texture* getTexture(TextureHandle handle);
	void removeTexture(TextureHandle handle);
	void loadPicture(TextureHandle handle, const Path& pictureFile);

	void runPendingSetups(GHIThreadCaller& caller);
	void runPendingCleanups(GHIThreadCaller& caller);

	void addResource(std::unique_ptr<ISceneResource> resource);
	void addDynamicResource(std::unique_ptr<IDynamicSceneResource> resource);
	void removeResource(ISceneResource* resource);
	void destroyRemovedResources();
	void updateDynamicResources(const RenderThreadUpdateContext& ctx);
	void createGHICommandsForDynamicResources(GHIThreadCaller& caller);

	System& getSystem();
	const std::string& getDebugName() const;
	void setSystem(System* sys);

	void removeAllContents();

public:
	ProjectiveView mainView;

private:
	System* m_sys;
	std::string m_debugName;

	template<typename Content, CWeakHandle Handle>
	using TContentPool = TItemPool<Content, TConcurrentHandleDispatcher<Handle>>;

	TContentPool<Texture, TextureHandle> m_textures;

	std::vector<TFunction<void(GHIThreadCaller& caller)>> m_pendingSetups;
	std::vector<TFunction<void(GHIThreadCaller& caller)>> m_pendingCleanups;

	struct DynamicResourceOrderer
	{
		bool operator () (const IDynamicSceneResource* a, const IDynamicSceneResource* b) const
		{
			return a->getProcessOrder() < b->getProcessOrder();
		}
	};

	TUniquePtrVector<ISceneResource> m_resources;
	std::vector<ISceneResource*> m_resourcesPendingDestroy;
	TSortedVector<IDynamicSceneResource*, DynamicResourceOrderer> m_dynamicResources;
};

inline System& Scene::getSystem()
{
	PH_ASSERT(m_sys);
	return *m_sys;
}

inline const std::string& Scene::getDebugName() const
{
	return m_debugName;
}

}// end namespace ph::editor::render
