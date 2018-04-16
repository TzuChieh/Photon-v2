#include "Actor/CookingContext.h"

namespace ph
{

CookingContext::CookingContext() : 
	m_childActors(), m_visualWorldInfo(nullptr)
{}

void CookingContext::addChildActor(std::unique_ptr<Actor> actor)
{
	m_childActors.push_back(std::move(actor));
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