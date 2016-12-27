#include "Core/CookedModelStorage.h"

namespace ph
{

void CookedModelStorage::clear()
{
	m_primitives.clear();
	m_primitiveMetadatas.clear();
	m_transforms.clear();
	m_surfaceBehaviors.clear();

	m_primitives.shrink_to_fit();
	m_primitiveMetadatas.shrink_to_fit();
	m_transforms.shrink_to_fit();
	m_surfaceBehaviors.shrink_to_fit();
}

void CookedModelStorage::add(std::unique_ptr<Primitive> primitive)
{
	m_primitives.push_back(std::move(primitive));
}

void CookedModelStorage::add(std::unique_ptr<PrimitiveMetadata> metadata)
{
	m_primitiveMetadatas.push_back(std::move(metadata));
}

void CookedModelStorage::add(std::unique_ptr<Transform> transform)
{
	m_transforms.push_back(std::move(transform));
}

void CookedModelStorage::add(std::unique_ptr<SurfaceBehavior> surfaceBehavior)
{
	m_surfaceBehaviors.push_back(std::move(surfaceBehavior));
}

void CookedModelStorage::add(std::vector<std::unique_ptr<Primitive>>&& primitives)
{
	m_primitives.insert(m_primitives.end(),
		std::make_move_iterator(primitives.begin()),
		std::make_move_iterator(primitives.end()));
}

void CookedModelStorage::add(CookedModelStorage&& other)
{
	m_primitives.insert(m_primitives.end(),
		std::make_move_iterator(other.m_primitives.begin()),
		std::make_move_iterator(other.m_primitives.end()));
	m_primitiveMetadatas.insert(m_primitiveMetadatas.end(),
		std::make_move_iterator(other.m_primitiveMetadatas.begin()),
		std::make_move_iterator(other.m_primitiveMetadatas.end()));
	m_transforms.insert(m_transforms.end(),
		std::make_move_iterator(other.m_transforms.begin()),
		std::make_move_iterator(other.m_transforms.end()));
	m_surfaceBehaviors.insert(m_surfaceBehaviors.end(),
		std::make_move_iterator(other.m_surfaceBehaviors.begin()),
		std::make_move_iterator(other.m_surfaceBehaviors.end()));
}

std::size_t CookedModelStorage::numPrimitives() const
{
	return m_primitives.size();
}

}// end namespace ph