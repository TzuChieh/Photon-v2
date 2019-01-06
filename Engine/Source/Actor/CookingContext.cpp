#include "Actor/CookingContext.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Cooking Context"));
}

CookingContext::CookingContext() : 
	m_childActors(), m_phantoms(), m_visualWorldInfo(nullptr)
{}

void CookingContext::addChildActor(std::unique_ptr<Actor> actor)
{
	m_childActors.push_back(std::move(actor));
}

void CookingContext::addPhantom(const std::string& name, CookedUnit phantom)
{
	if(m_phantoms.find(name) != m_phantoms.end())
	{
		logger.log(ELogLevel::WARNING_MED, 
			"phantom name <" + name + "> already exists, overwriting");
	}

	m_phantoms[name] = std::move(phantom);
}

const CookedUnit* CookingContext::getPhantom(const std::string& name) const
{
	const auto result = m_phantoms.find(name);
	return result != m_phantoms.end() ? &(result->second) : nullptr;
}

void CookingContext::setVisualWorldInfo(const VisualWorldInfo* const info)
{
	m_visualWorldInfo = info;
}

std::vector<std::unique_ptr<Actor>> CookingContext::claimChildActors()
{
	auto childActors = std::move(m_childActors);
	m_childActors.clear();
	return childActors;
}

}// end namespace ph