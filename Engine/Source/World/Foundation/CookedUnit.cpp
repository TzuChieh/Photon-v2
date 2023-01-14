#include "World/Foundation/CookedUnit.h"
#include "Common/assertion.h"
#include "World/Foundation/CookedDataStorage.h"

namespace ph
{

CookedUnit::CookedUnit() :
	m_intersectables(), 
	m_primitiveMetadata(), 
	m_transforms(),
	m_emitter(),
	m_backendIntersectables()
{}

CookedUnit::CookedUnit(CookedUnit&& other) :
	m_intersectables       (std::move(other.m_intersectables)),
	m_primitiveMetadata    (std::move(other.m_primitiveMetadata)),
	m_transforms           (std::move(other.m_transforms)),
	m_emitter              (std::move(other.m_emitter)),
	m_backendIntersectables(std::move(other.m_backendIntersectables))
{}

void CookedUnit::addIntersectable(std::unique_ptr<Intersectable> intersectable)
{
	PH_ASSERT(intersectable);

	m_intersectables.push_back(std::move(intersectable));
}

void CookedUnit::setPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata)
{
	PH_ASSERT(metadata);

	m_primitiveMetadata = std::move(metadata);
}

void CookedUnit::addTransform(std::unique_ptr<math::Transform> transform)
{
	PH_ASSERT(transform);

	m_transforms.push_back(std::move(transform));
}

void CookedUnit::setEmitter(std::unique_ptr<Emitter> emitter)
{
	PH_ASSERT(emitter);

	m_emitter = std::move(emitter);
}

void CookedUnit::claimCookedData(CookedDataStorage& storage)
{
	storage.add(std::move(m_intersectables));
	storage.add(std::move(m_primitiveMetadata));
	storage.add(std::move(m_transforms));
	storage.add(std::move(m_emitter));

	m_intersectables.clear();
	m_primitiveMetadata = nullptr;
	m_transforms.clear();
	m_emitter = nullptr;
}

void CookedUnit::addBackend(std::unique_ptr<Intersectable> intersectable)
{
	PH_ASSERT(intersectable);

	m_backendIntersectables.push_back(std::move(intersectable));
}

void CookedUnit::claimCookedBackend(CookedDataStorage& storage)
{
	storage.add(std::move(m_backendIntersectables));

	m_backendIntersectables.clear();
}

CookedUnit& CookedUnit::operator = (CookedUnit&& rhs)
{
	m_intersectables        = std::move(rhs.m_intersectables);
	m_primitiveMetadata     = std::move(rhs.m_primitiveMetadata);
	m_transforms            = std::move(rhs.m_transforms);
	m_emitter               = std::move(rhs.m_emitter);
	m_backendIntersectables = std::move(rhs.m_backendIntersectables);

	return *this;
}

}// end namespace ph
