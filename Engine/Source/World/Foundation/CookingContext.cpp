#include "World/Foundation/CookingContext.h"
#include "Common/logging.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/VisualWorld.h"
#include "Actor/Geometry/Geometry.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext(const VisualWorld* const world)
	: m_config()
	, m_world(world)
	, m_resources(nullptr)
	, m_cache(nullptr)

	, m_childActors        ()
	, m_phantoms           ()
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

void CookingContext::addChildActor(std::unique_ptr<Actor> actor)
{
	m_childActors.push_back(std::move(actor));
}

void CookingContext::addPhantom(const std::string& name, TransientVisualElement phantom)
{
	if(m_phantoms.find(name) != m_phantoms.end())
	{
		PH_LOG_WARNING(CookingContext, 
			"phantom name <{}> already exists, overwriting", name);
	}

	m_phantoms[name] = std::move(phantom);
}

const TransientVisualElement* CookingContext::getPhantom(const std::string& name) const
{
	const auto result = m_phantoms.find(name);
	return result != m_phantoms.end() ? &(result->second) : nullptr;
}

std::vector<std::unique_ptr<Actor>> CookingContext::claimChildActors()
{
	auto childActors = std::move(m_childActors);
	m_childActors.clear();
	return childActors;
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
