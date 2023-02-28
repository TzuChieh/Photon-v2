#pragma once

#include "Utility/IMoveOnly.h"
#include "Actor/Actor.h"
#include "World/Foundation/CookedUnit.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Geometry/TAABB3D.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "DataIO/SDL/sdl_traits.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <utility>

namespace ph
{

class VisualWorld;
class CookedResourceCollection;
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
	void addChildActor(std::unique_ptr<Actor> actor);

	void addPhantom(const std::string& name, CookedUnit phantom);
	const CookedUnit* getPhantom(const std::string& name) const;

	std::vector<std::unique_ptr<Actor>> claimChildActors();

	CookedResourceCollection* getResources() const;

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

private:
	const VisualWorld& getWorld() const;

	const VisualWorld* m_world;
	CookedResourceCollection* m_resources;
	std::vector<std::unique_ptr<Actor>>         m_childActors;
	std::unordered_map<std::string, CookedUnit> m_phantoms;
};

}// end namespace ph
