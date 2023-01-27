#pragma once

#include "Utility/IMoveOnly.h"
#include "Actor/Actor.h"
#include "World/Foundation/CookedUnit.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Geometry/TAABB3D.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <utility>

namespace ph
{

class VisualWorld;
class CookedResourceCollection;

/*! @brief Information about the world being cooked.
*/
class CookingContext final : private IMoveOnly
{
	// TODO: remove
	friend class VisualWorld;

public:
	explicit CookingContext(VisualWorld* world);

	// TODO: we can assign child actors special attributes such as
	// deferred cooking, which opens the possibility of calculating
	// full scene bound before cooking (or their parent actor)
	void addChildActor(std::unique_ptr<Actor> actor);

	void addPhantom(const std::string& name, CookedUnit phantom);
	const CookedUnit* getPhantom(const std::string& name) const;

	std::vector<std::unique_ptr<Actor>> claimChildActors();

	void setBackgroundPrimitive(std::unique_ptr<Primitive> primitive);
	std::unique_ptr<Primitive> claimBackgroundPrimitive();

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

private:
	VisualWorld& getWorld();
	const VisualWorld& getWorld() const;

	VisualWorld* m_world;
	CookedResourceCollection* m_resources;
	std::vector<std::unique_ptr<Actor>>         m_childActors;
	std::unordered_map<std::string, CookedUnit> m_phantoms;
	std::unique_ptr<Primitive>                  m_backgroundPrimitive;
};

inline void CookingContext::setBackgroundPrimitive(std::unique_ptr<Primitive> primitive)
{
	PH_ASSERT_MSG(!m_backgroundPrimitive, 
		"Cannot overwrite existing background primitive");

	m_backgroundPrimitive = std::move(primitive);
}

inline std::unique_ptr<Primitive> CookingContext::claimBackgroundPrimitive()
{
	return std::move(m_backgroundPrimitive);
}

}// end namespace ph
