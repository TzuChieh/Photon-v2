#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <format>

namespace ph
{

CookedResourceCollection::~CookedResourceCollection() = default;

TSynchronized<CookedNamedResource>& CookedResourceCollection::getNamed()
{
	return m_namedResource;
}

const CookedGeometry* CookedResourceCollection::getGeometry(const CookedResourceKey& key) const
{
	return getCookedResourceByKey(m_keyToGeometry, key);
}

const CookedMaterial* CookedResourceCollection::getMaterial(const CookedResourceKey& key) const
{
	return getCookedResourceByKey(m_keyToMaterial, key);
}

const CookedMotion* CookedResourceCollection::getMotion(const CookedResourceKey& key) const
{
	return getCookedResourceByKey(m_keyToMotion, key);
}

std::string CookedResourceCollection::getStats() const
{
	return std::format(
		"{} metadatas, {} transforms, {} intersectables, {} emitters, {} triangle buffers, "
		"{} geometries, {} materials, {} motions",
		m_metadatas->size(),
		m_transforms->size(), 
		m_intersectables->size(),
		m_emitters->size(),
		m_triangleBuffers->size(),
		m_keyToGeometry->size(),
		m_keyToMaterial->size(),
		m_keyToMotion->size());
}

}// end namespace ph
