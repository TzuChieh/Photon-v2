#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "World/Foundation/CookingConfig.h"

#include <Common/assertion.h>

#include <memory>

namespace ph
{

class VisualWorld;
class CookedResourceCollection;
class TransientResourceCache;
class TransientVisualElement;
class Geometry;
class Actor;
class CookedGeometry;

/*! @brief Information about the world being cooked.
*/
class CookingContext final
{
	// TODO: remove
	friend class VisualWorld;

public:
	explicit CookingContext(const VisualWorld* world);

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

	/*!
	@return Pointer to the cooked geometry. `nullptr` if not found.
	*/
	const CookedGeometry* getCooked(const std::shared_ptr<Geometry>& geometry) const;

	/*!
	@return Pointer to the cooked actor. `nullptr` if not found.
	*/
	const TransientVisualElement* getCached(const std::shared_ptr<Actor>& actor) const;

private:
	const VisualWorld& getWorld() const;

	CookingConfig m_config;
	const VisualWorld* m_world;
	CookedResourceCollection* m_resources;
	TransientResourceCache* m_cache;
};

}// end namespace ph
