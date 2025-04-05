#pragma once

#include "Engine/World/Foundation/CookedDataStorage.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Actor/Actor.h"
#include "Engine/World/Scene.h"
#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/Emitter/Sampler/EmitterSampler.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Utility/IMoveOnly.h"
#include "Engine/EngineEnv/EAccelerator.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/TransientResourceCache.h"
#include "Engine/Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>
#include <memory>

namespace ph { class SceneDescription; }
namespace ph { class CoreCookingContext; }
namespace ph { class CookingContext; }
namespace ph { class Primitive; }

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

	/*! @brief Bounds actors cooked in the first level.
	The bound is only available after the first level has done cooking.
	*/
	math::AABB3D getRootActorsBound() const;

	/*! @brief Bounds actors from levels finished cooking.
	The bound is updated every time a level has done cooking. Generally this bound only grows as it
	encapsulates all previous levels including the root level.
	*/
	math::AABB3D getLeafActorsBound() const;

private:
	struct SceneActor
	{
		std::shared_ptr<Actor> actor;
		bool isPhantom = false;
	};

	std::unique_ptr<CookedResourceCollection> m_cookedResources;
	std::unique_ptr<TransientResourceCache> m_cache;
	std::vector<TransientVisualElement> m_cookedUnits;
	math::Vector3R m_receiverPos;
	math::AABB3D m_rootActorsBound;
	math::AABB3D m_leafActorsBound;

	std::unique_ptr<Intersector>    m_tlas;
	std::unique_ptr<EmitterSampler> m_emitterSampler;
	std::unique_ptr<Scene>          m_scene;
	const Primitive* m_backgroundPrimitive;

	void cookActors(
		TSpan<SceneActor> sceneActors,
		CookingContext& ctx,
		std::vector<TransientVisualElement>& out_elements);

	static std::unique_ptr<Intersector> createTopLevelAccelerator(
		EAccelerator acceleratorType,
		TSpanView<const Intersectable*> intersectables);

	static math::AABB3D calcElementBound(TSpanView<TransientVisualElement> elements);
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
