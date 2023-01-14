#include "World/Foundation/CookingContext.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext() :
	m_childActors        (), 
	m_phantoms           (), 
	m_visualWorldInfo    (nullptr),
	m_backgroundPrimitive(nullptr)
{}

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
