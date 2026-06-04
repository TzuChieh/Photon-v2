#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/VisualWorld.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Actor.h"
#include "Engine/World/Foundation/TransientVisualElement.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>
#include <type_traits>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext(
	CookedResourceCollection* const resources,
	TransientResourceCache* const cache)
	: m_config()
	, m_world(nullptr)
	, m_resources(resources)
	, m_cache(cache)
{
	PH_LOG(CookingContext, Note,
		"created context, contains resource storage: {}, contains transient cache: {}",
		m_resources != nullptr, m_cache != nullptr);
}

CookingContext::CookingContext(const VisualWorld* const world)
	: CookingContext(
		world ? world->getCookedResources() : nullptr,
		world ? world->getCache() : nullptr)
{
	m_world = world;
}

const CookingConfig& CookingContext::getConfig() const
{
	return m_config;
}

void CookingContext::setConfig(CookingConfig config)
{
	m_config = std::move(config);
}

CookedResourceCollection& CookingContext::getResources() const
{
	PH_ASSERT(m_resources);
	return *m_resources;
}

TransientResourceCache& CookingContext::getCache() const
{
	PH_ASSERT(m_cache);
	return *m_cache;
}

math::AABB3D CookingContext::getRootActorsBound() const
{
	return getWorld().getRootActorsBound();
}

math::AABB3D CookingContext::getLeafActorsBound() const
{
	return getWorld().getLeafActorsBound();
}

const CookedGeometry* CookingContext::getCooked(const std::shared_ptr<Geometry>& geometry) const
{
	return geometry != nullptr
		? getResources().getGeometry(geometry->getId())
		: nullptr;
}

const TransientVisualElement* CookingContext::getCached(const std::shared_ptr<Actor>& actor) const
{
	return actor != nullptr
		? getCache().getVisualElement(actor->getId())
		: nullptr;
}

const VisualWorld& CookingContext::getWorld() const
{
	PH_ASSERT(m_world);
	return *m_world;
}

static_assert(std::is_copy_constructible_v<CookingContext>,
	"CookingContext must be copyable so modified context can be easily created.");

}// end namespace ph
