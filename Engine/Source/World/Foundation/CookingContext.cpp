#include "World/Foundation/CookingContext.h"
#include "Common/logging.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/VisualWorld.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Actor.h"
#include "World/Foundation/TransientVisualElement.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext(const VisualWorld* const world)
	: m_config()
	, m_world(world)
	, m_resources(nullptr)
	, m_cache(nullptr)
{
	if(world)
	{
		m_resources = getWorld().getCookedResources();
		m_cache = getWorld().getCache();
	}
	
	PH_LOG(CookingContext, 
		"created context, contains resource storage: {}, contains transient cache: {}",
		m_resources != nullptr, m_cache != nullptr);
}

const CookingConfig& CookingContext::getConfig() const
{
	return m_config;
}

void CookingContext::setConfig(CookingConfig config)
{
	m_config = std::move(config);
}

CookedResourceCollection* CookingContext::getResources() const
{
	return m_resources;
}

TransientResourceCache* CookingContext::getCache() const
{
	return m_cache;
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
		? getResources()->getGeometry(geometry->getId())
		: nullptr;
}

const TransientVisualElement* CookingContext::getCached(const std::shared_ptr<Actor>& actor) const
{
	return actor != nullptr
		? getCache()->getVisualElement(actor->getId())
		: nullptr;
}

const VisualWorld& CookingContext::getWorld() const
{
	PH_ASSERT(m_world);
	return *m_world;
}

}// end namespace ph
