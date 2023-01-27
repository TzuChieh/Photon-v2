#pragma once

#include "Common/primitive_type.h"
#include "Core/Intersectable/Primitive.h"
#include "World/Foundation/CookedDataStorage.h"
#include "World/Foundation/CookedUnit.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Utility/IMoveOnly.h"
#include "EngineEnv/EAccelerator.h"
#include "World/Foundation/CookedResourceCollection.h"

#include <vector>
#include <memory>

namespace ph { class SceneDescription; }
namespace ph { class CoreCookingContext; }
namespace ph { class CookingContext; }

namespace ph
{

class VisualWorld final : private IMoveOnly
{
public:
	VisualWorld();

	void cook(const SceneDescription& rawScene, const CoreCookingContext& coreCtx);

	// HACK
	void setReceiverPosition(const math::Vector3R& receiverPos);

	const Intersector* getIntersector() const;
	const EmitterSampler* getEmitterSampler() const;
	const Scene* getScene() const;
	CookedResourceCollection* getCookedResources();

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
	std::unique_ptr<CookedResourceCollection> m_cookedResources;
	std::vector<CookedUnit> m_cookedUnits;
	CookedDataStorage m_cookedActorStorage;
	CookedDataStorage m_cookedBackendStorage;
	CookedDataStorage m_phantomStorage;
	math::Vector3R m_receiverPos;
	math::AABB3D m_rootActorsBound;
	math::AABB3D m_leafActorsBound;

	std::unique_ptr<Intersector>    m_intersector;
	std::unique_ptr<EmitterSampler> m_emitterSampler;
	std::unique_ptr<Scene>          m_scene;
	std::unique_ptr<Primitive>      m_backgroundPrimitive;

	void cookActors(
		std::shared_ptr<Actor>* actors, 
		std::size_t numActors, 
		CookingContext& ctx);

	void createTopLevelAccelerator(EAccelerator acceleratorType);

	static math::AABB3D calcIntersectableBound(const CookedDataStorage& storage);
};

// In-header Implementations:

inline void VisualWorld::setReceiverPosition(const math::Vector3R& receiverPos)
{
	m_receiverPos = receiverPos;
}

inline const Intersector* VisualWorld::getIntersector() const
{
	return m_intersector.get();
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
