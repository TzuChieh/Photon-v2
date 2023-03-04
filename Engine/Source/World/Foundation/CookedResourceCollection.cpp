#include "World/Foundation/CookedResourceCollection.h"

#include <format>

namespace ph
{

TSynchronized<CookedNamedResource>& CookedResourceCollection::getNamed()
{
	return m_namedResource;
}

const CookedGeometry* CookedResourceCollection::getGeometry(const SdlResourceId id) const
{
	return getCookedResourceByID(m_idToGeometry, id);
}

const CookedMotion* CookedResourceCollection::getMotion(const SdlResourceId id) const
{
	return getCookedResourceByID(m_idToMotion, id);
}

std::string CookedResourceCollection::getStats() const
{
	return std::format(
		"{} metadatas, {} transforms, {} intersectables, {} geometries, {} motions",
		m_metadatas->size(),
		m_transforms->size(), 
		m_intersectables->size(),
		m_idToGeometry->size(),
		m_idToMotion->size());
}

}// end namespace ph
