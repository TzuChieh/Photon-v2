#pragma once

#include "Common/primitive_type.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/CookedDataStorage.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Common/Logger.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"
#include "World/CookSettings.h"
#include "Common/assertion.h"
#include "Utility/IMoveOnly.h"

#include <vector>
#include <memory>

namespace ph
{

class CookingContext;

// TODO: allow copy?
class VisualWorld final : public IMoveOnly
{
public:
	VisualWorld();
	VisualWorld(VisualWorld&& other);

	void cook();
	void addActor(std::shared_ptr<Actor> actor);

	// HACK
	void setReceiverPosition(const math::Vector3R& receiverPos);

	void setCookSettings(const CookSettings& settings);

	std::shared_ptr<Intersector> getIntersector() const;
	std::shared_ptr<EmitterSampler> getEmitterSampler() const;
	std::shared_ptr<Scene> getScene() const;

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
	CookedDataStorage m_cookedActorStorage;
	CookedDataStorage m_cookedBackendStorage;
	CookedDataStorage m_phantomStorage;
	math::Vector3R m_receiverPos;
	CookSettings m_cookSettings;

	std::shared_ptr<Intersector>    m_intersector;
	std::shared_ptr<EmitterSampler> m_emitterSampler;
	std::shared_ptr<Scene>          m_scene;
	std::shared_ptr<Primitive>      m_backgroundPrimitive;

	void cookActors(
		std::shared_ptr<Actor>* actors, 
		std::size_t             numActors, 
		CookingContext&         cookingContext);

	void createTopLevelAccelerator();

	static math::AABB3D calcIntersectableBound(const CookedDataStorage& storage);

	static const Logger logger;
};

// In-header Implementations:

inline void VisualWorld::setReceiverPosition(const math::Vector3R& receiverPos)
{
	m_receiverPos = receiverPos;
}

inline void VisualWorld::setCookSettings(const CookSettings& settings)
{
	m_cookSettings = settings;
}

inline std::shared_ptr<Intersector> VisualWorld::getIntersector() const
{
	return m_intersector;
}

inline std::shared_ptr<EmitterSampler> VisualWorld::getEmitterSampler() const
{
	return m_emitterSampler;
}

inline std::shared_ptr<Scene> VisualWorld::getScene() const
{
	return m_scene;
}

}// end namespace ph
