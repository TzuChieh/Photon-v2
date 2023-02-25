#include "World/Foundation/CookingContext.h"
#include "Common/logging.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/VisualWorld.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext(const VisualWorld* const world)
	: m_world(world)
	, m_resources(nullptr)
	, m_childActors        ()
	, m_phantoms           ()
{
	m_resources = getWorld().getCookedResources();
	if(!m_resources)
	{
		PH_LOG(CookingContext, "cooked resource storage is empty");
	}
}

void CookingContext::addChildActor(std::unique_ptr<Actor> actor)
{
	m_childActors.push_back(std::move(actor));
}

void CookingContext::addPhantom(const std::string& name, CookedUnit phantom)
{
	if(m_phantoms.find(name) != m_phantoms.end())
	{
		PH_LOG_WARNING(CookingContext, 
			"phantom name <{}> already exists, overwriting", name);
	}

	m_phantoms[name] = std::move(phantom);
}

const CookedUnit* CookingContext::getPhantom(const std::string& name) const
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

CookedResourceCollection* CookingContext::getCooked() const
{
	return m_resources;
}

math::AABB3D CookingContext::getRootActorsBound() const
{
	return getWorld().getRootActorsBound();
}

math::AABB3D CookingContext::getLeafActorsBound() const
{
	return getWorld().getLeafActorsBound();
}

const VisualWorld& CookingContext::getWorld() const
{
	PH_ASSERT(m_world);
	return *m_world;
}

}// end namespace ph
