#include "Actor/CookedDataStorage.h"

namespace ph
{

CookedDataStorage::CookedDataStorage(CookedDataStorage&& other) :
	CookedDataStorage()
{
	add(std::move(other));
}

void CookedDataStorage::clear()
{
	m_intersectables.clear();
	m_intersectables.shrink_to_fit();

	m_primitiveMetadatas.clear();
	m_primitiveMetadatas.shrink_to_fit();

	m_emitters.clear();
	m_emitters.shrink_to_fit();

	m_transforms.clear();
	m_transforms.shrink_to_fit();
}

void CookedDataStorage::add(std::unique_ptr<Intersectable> intersectable)
{
	if(intersectable != nullptr)
	{
		m_intersectables.push_back(std::move(intersectable));
	}
}

void CookedDataStorage::add(std::unique_ptr<PrimitiveMetadata> metadata)
{
	if(metadata != nullptr)
	{
		m_primitiveMetadatas.push_back(std::move(metadata));
	}
}

void CookedDataStorage::add(std::unique_ptr<Emitter> emitter)
{
	if(emitter != nullptr)
	{
		m_emitters.push_back(std::move(emitter));
	}
}

void CookedDataStorage::add(std::unique_ptr<Transform> transform)
{
	if(transform != nullptr)
	{
		m_transforms.push_back(std::move(transform));
	}
}

void CookedDataStorage::add(std::vector<std::unique_ptr<Intersectable>>&& intersectables)
{
	for(auto& intersectable : intersectables)
	{
		add(std::move(intersectable));
	}
}

void CookedDataStorage::add(std::vector<std::unique_ptr<Transform>>&& transforms)
{
	for(auto& transform : transforms)
	{
		add(std::move(transform));
	}
}

void CookedDataStorage::add(CookedDataStorage&& other)
{
	m_intersectables.insert(m_intersectables.end(),
		std::make_move_iterator(other.m_intersectables.begin()),
		std::make_move_iterator(other.m_intersectables.end()));
	m_primitiveMetadatas.insert(m_primitiveMetadatas.end(),
		std::make_move_iterator(other.m_primitiveMetadatas.begin()),
		std::make_move_iterator(other.m_primitiveMetadatas.end()));
	m_emitters.insert(m_emitters.end(),
		std::make_move_iterator(other.m_emitters.begin()),
		std::make_move_iterator(other.m_emitters.end()));
	m_transforms.insert(m_transforms.end(),
		std::make_move_iterator(other.m_transforms.begin()),
		std::make_move_iterator(other.m_transforms.end()));
}

std::size_t CookedDataStorage::numIntersectables() const
{
	return m_intersectables.size();
}

std::size_t CookedDataStorage::numEmitters() const
{
	return m_emitters.size();
}

}// end namespace ph