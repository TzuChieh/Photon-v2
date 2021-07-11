#include "Actor/ActorCookingContext.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Actor Cooking Context"));
}

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
		logger.log(ELogLevel::WARNING_MED, 
			"phantom name <" + name + "> already exists, overwriting");
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
