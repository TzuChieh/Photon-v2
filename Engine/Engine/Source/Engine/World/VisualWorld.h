#pragma once

#include "Engine/World/Scene.h"
#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/Emitter/Sampler/EmitterSampler.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Utility/IMoveOnly.h"
#include "Engine/EngineEnv/EAccelerator.h"
#include "Engine/World/Foundation/CookOrder.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/TransientResourceCache.h"
#include "Engine/Utility/TSpan.h"

#include <memory>
#include <optional>

namespace ph { class Actor; }
namespace ph { class SceneDescription; }
namespace ph { class CoreCookingContext; }
namespace ph { class CookingContext; }
namespace ph { class ISdlResource; }
namespace ph { class Primitive; }
namespace ph { class TransientVisualElement; }

namespace ph
{

/*! @brief A virtual world for image synthesis.
*/
class VisualWorld final : private IMoveOnly
{
public:
	VisualWorld();

	void cook(const SceneDescription& rawScene, const CoreCookingContext& coreCtx);

	// HACK
	void setReceiverPosition(const math::Vector3R& receiverPos);

	/*!
	@return The top-level acceleration structure of this world.
	*/
	const Intersector* getTLAS() const;

	const EmitterSampler* getEmitterSampler() const;
	const Scene* getScene() const;
	CookedResourceCollection* getCookedResources() const;
	TransientResourceCache* getCache() const;

	/*! @brief Get the bound of successfully cooked, scene-visible first-level actors.
	The bound includes the receiver position and is published after `ECookLevel::First` finishes. It
	remains unchanged during later levels.
	*/
	math::AABB3D getRootActorsBound() const;

	/*! @brief Get the cumulative bound through the last completed actor cook level.
	The bound includes the receiver position and successfully cooked, scene-visible actors from
	completed levels only. All actors within the same level observe the same bound.
	*/
	math::AABB3D getAllActorsBound() const;

private:
	struct ResourceCookUnit
	{
		const ISdlResource* resource = nullptr;
		const TransientVisualElement* visibleElement = nullptr;
		std::optional<CookLevel> actorCookLevel;
		bool isPhantom = false;
	};

	std::unique_ptr<CookedResourceCollection> m_cookedResources;
	std::unique_ptr<TransientResourceCache> m_cache;
	math::Vector3R m_receiverPos;
	math::AABB3D m_rootActorsBound;
	math::AABB3D m_allActorsBound;

	std::unique_ptr<Intersector> m_tlas;
	std::unique_ptr<EmitterSampler> m_emitterSampler;
	std::unique_ptr<Scene> m_scene;
	const Primitive* m_backgroundPrimitive;

	const TransientVisualElement* cookActor(const Actor& actor, CookingContext& ctx);
	void onFinishedActorCookLevel(CookLevel level, TSpanView<ResourceCookUnit> levelCookUnits);

	static std::unique_ptr<Intersector> createTopLevelAccelerator(
		EAccelerator acceleratorType,
		TSpanView<const Intersectable*> intersectables);
};

// In-header Implementations:

inline void VisualWorld::setReceiverPosition(const math::Vector3R& receiverPos)
{
	m_receiverPos = receiverPos;
}

inline const Intersector* VisualWorld::getTLAS() const
{
	return m_tlas.get();
}

inline const EmitterSampler* VisualWorld::getEmitterSampler() const
{
	return m_emitterSampler.get();
}

inline const Scene* VisualWorld::getScene() const
{
	return m_scene.get();
}

}// end namespace ph
