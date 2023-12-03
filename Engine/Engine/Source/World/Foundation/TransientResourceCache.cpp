#include "World/Foundation/TransientResourceCache.h"

#include <format>

namespace ph
{

const TransientVisualElement* TransientResourceCache::getVisualElement(const SdlResourceId id) const
{
	return getCookedResourceByID(m_idToVisualElement, id);
}

std::string TransientResourceCache::getStats() const
{
	return std::format(
		"{} visual elements",
		m_idToVisualElement->size());
}

}// end namespace ph
