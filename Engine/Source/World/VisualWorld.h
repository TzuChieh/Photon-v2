#pragma once

#include "Common/primitive_type.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/CookedDataStorage.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Common/Logger.h"
#include "Core/Bound/AABB3D.h"
#include "Math/TVector3.h"

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
	void setCameraPosition(const Vector3R& cameraPos);

	const Scene& getScene() const;

	// forbid copying
	VisualWorld(const VisualWorld& other) = delete;
	VisualWorld& operator = (const VisualWorld& rhs) = delete;

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
	CookedDataStorage m_cookedActorStorage;
	CookedDataStorage m_cookedBackendStorage;
	Vector3R m_cameraPos;

	std::unique_ptr<Intersector>  m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;
	Scene                         m_scene;
	
	// HACK
	const Primitive* m_backgroundEmitterPrimitive;

	void cookActors(CookingContext& cookingContext);

	static AABB3D calcIntersectableBound(const CookedDataStorage& storage);

	static const Logger logger;
};

// In-header Implementations:

inline void VisualWorld::setCameraPosition(const Vector3R& cameraPos)
{
	m_cameraPos = cameraPos;
}

}// end namespace ph