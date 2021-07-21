#pragma once

#include "Common/primitive_type.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/CookedDataStorage.h"
#include "Actor/CookedUnit.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Common/Logger.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Utility/IMoveOnly.h"
#include "EngineEnv/EAccelerator.h"

#include <vector>
#include <memory>

namespace ph { class SceneDescription; }
namespace ph { class CoreCookingContext; }
namespace ph { class ActorCookingContext; }

namespace ph
{

class VisualWorld final : public IMoveOnly
{
public:
	VisualWorld();

	void cook(const SceneDescription& rawScene, const CoreCookingContext& coreCtx);

	// HACK
	void setReceiverPosition(const math::Vector3R& receiverPos);

	const Intersector* getIntersector() const;
	const EmitterSampler* getEmitterSampler() const;
	const Scene* getScene() const;

private:
	std::vector<CookedUnit> m_cookedUnits;
	CookedDataStorage m_cookedActorStorage;
	CookedDataStorage m_cookedBackendStorage;
	CookedDataStorage m_phantomStorage;
	math::Vector3R m_receiverPos;

	std::unique_ptr<Intersector>    m_intersector;
	std::unique_ptr<EmitterSampler> m_emitterSampler;
	std::unique_ptr<Scene>          m_scene;
	std::unique_ptr<Primitive>      m_backgroundPrimitive;

	void cookActors(
		std::shared_ptr<Actor>* actors, 
		std::size_t             numActors, 
		ActorCookingContext&    ctx);

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
