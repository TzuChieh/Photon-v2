#include "Actor/ActorCookingContext.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ActorCookingContext, Actor);

ActorCookingContext::ActorCookingContext() :
	m_childActors        (), 
	m_phantoms           (), 
	m_visualWorldInfo    (nullptr),
	m_backgroundPrimitive(nullptr)
{}

void ActorCookingContext::addChildActor(std::unique_ptr<Actor> actor)
{
	m_childActors.push_back(std::move(actor));
}

void ActorCookingContext::addPhantom(const std::string& name, CookedUnit phantom)
{
	if(m_phantoms.find(name) != m_phantoms.end())
	{
		PH_LOG_WARNING(ActorCookingContext, 
			"phantom name <{}> already exists, overwriting", name);
	}

	m_phantoms[name] = std::move(phantom);
}

const CookedUnit* ActorCookingContext::getPhantom(const std::string& name) const
{
	const auto result = m_phantoms.find(name);
	return result != m_phantoms.end() ? &(result->second) : nullptr;
}

void ActorCookingContext::setVisualWorldInfo(const VisualWorldInfo* const info)
{
	m_visualWorldInfo = info;
}

std::vector<std::unique_ptr<Actor>> ActorCookingContext::claimChildActors()
{
	auto childActors = std::move(m_childActors);
	m_childActors.clear();
	return childActors;
}

}// end namespace ph
