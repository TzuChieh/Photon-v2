#pragma once

#include "Utility/IMoveOnly.h"
#include "Actor/Actor.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Geometry/TAABB3D.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "SDL/sdl_traits.h"
#include "World/Foundation/CookingConfig.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace ph
{

class VisualWorld;
class CookedResourceCollection;
class TransientResourceCache;
class Geometry;
class CookedGeometry;

/*! @brief Information about the world being cooked.
*/
class CookingContext final : private IMoveOnly
{
	// TODO: remove
	friend class VisualWorld;

public:
	explicit CookingContext(const VisualWorld* world);

	// TODO: we can assign child actors special attributes such as
	// deferred cooking, which opens the possibility of calculating
	// full scene bound before cooking (or their parent actor)
	// DEPRECATED
	void addChildActor(std::unique_ptr<Actor> actor);

	// DEPRECATED
	void addPhantom(const std::string& name, TransientVisualElement phantom);

	// DEPRECATED
	const TransientVisualElement* getPhantom(const std::string& name) const;

	// DEPRECATED
	std::vector<std::unique_ptr<Actor>> claimChildActors();

	const CookingConfig& getConfig() const;
	void setConfig(CookingConfig config);
	CookedResourceCollection* getResources() const;
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

	const CookedGeometry* getCooked(const std::shared_ptr<Geometry>& geometry) const;
	const TransientVisualElement* getCached(const std::shared_ptr<Actor>& actor) const;

private:
	const VisualWorld& getWorld() const;

	CookingConfig m_config;
	const VisualWorld* m_world;
	CookedResourceCollection* m_resources;
	TransientResourceCache* m_cache;

	std::vector<std::unique_ptr<Actor>>         m_childActors;
	std::unordered_map<std::string, TransientVisualElement> m_phantoms;
};

}// end namespace ph
