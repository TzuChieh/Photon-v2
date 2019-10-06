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

#include <vector>
#include <memory>

namespace ph
{

class CookingContext;

class VisualWorld final
{
public:
	VisualWorld();
	VisualWorld(VisualWorld&& other);

	void cook();
	void addActor(std::shared_ptr<Actor> actor);

	// HACK
	void setCameraPosition(const Vector3R& cameraPos);

	void setCookSettings(const std::shared_ptr<CookSettings>& settings);

	const Scene& getScene() const;

	// forbid copying
	VisualWorld(const VisualWorld& other) = delete;
	VisualWorld& operator = (const VisualWorld& rhs) = delete;

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
	CookedDataStorage m_cookedActorStorage;
	CookedDataStorage m_cookedBackendStorage;
	CookedDataStorage m_phantomStorage;
	Vector3R m_cameraPos;

	std::unique_ptr<Intersector>    m_intersector;
	std::unique_ptr<EmitterSampler> m_emitterSampler;
	Scene                           m_scene;
	std::shared_ptr<CookSettings>   m_cookSettings;
	
	// HACK
	const Primitive* m_backgroundEmitterPrimitive;

	void cookActors(CookingContext& cookingContext);
	void createTopLevelAccelerator();

	static math::AABB3D calcIntersectableBound(const CookedDataStorage& storage);

	static const Logger logger;
};

// In-header Implementations:

inline void VisualWorld::setCameraPosition(const Vector3R& cameraPos)
{
	m_cameraPos = cameraPos;
}

inline void VisualWorld::setCookSettings(const std::shared_ptr<CookSettings>& settings)
{
	PH_ASSERT(settings);

	m_cookSettings = settings;
}

}// end namespace ph