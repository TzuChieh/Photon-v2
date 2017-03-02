#include "Core/CookedActorStorage.h"

namespace ph
{

void CookedActorStorage::clear()
{
	m_primitives.clear();
	m_primitiveMetadatas.clear();
	m_emitters.clear();

	m_primitives.shrink_to_fit();
	m_primitiveMetadatas.shrink_to_fit();
	m_emitters.shrink_to_fit();
}

void CookedActorStorage::add(std::unique_ptr<Primitive> primitive)
{
	if(primitive != nullptr)
	{
		m_primitives.push_back(std::move(primitive));
	}
}

void CookedActorStorage::add(std::unique_ptr<PrimitiveMetadata> metadata)
{
	if(metadata != nullptr)
	{
		m_primitiveMetadatas.push_back(std::move(metadata));
	}
}

void CookedActorStorage::add(std::unique_ptr<Emitter> emitter)
{
	if(emitter != nullptr)
	{
		m_emitters.push_back(std::move(emitter));
	}
}

void CookedActorStorage::add(CookedActor&& cookedActor)
{
	add(std::move(cookedActor.primitives));
	add(std::move(cookedActor.primitiveMetadata));
	add(std::move(cookedActor.emitter));
}

void CookedActorStorage::add(std::vector<std::unique_ptr<Primitive>>&& primitives)
{
	for(auto& primitive : primitives)
	{
		if(primitive != nullptr)
		{
			m_primitives.push_back(std::move(primitive));
		}
	}
}

void CookedActorStorage::add(CookedActorStorage&& other)
{
	m_primitives.insert(m_primitives.end(),
		std::make_move_iterator(other.m_primitives.begin()),
		std::make_move_iterator(other.m_primitives.end()));
	m_primitiveMetadatas.insert(m_primitiveMetadatas.end(),
		std::make_move_iterator(other.m_primitiveMetadatas.begin()),
		std::make_move_iterator(other.m_primitiveMetadatas.end()));
	m_emitters.insert(m_emitters.end(),
		std::make_move_iterator(other.m_emitters.begin()),
		std::make_move_iterator(other.m_emitters.end()));
}

std::size_t CookedActorStorage::numPrimitives() const
{
	return m_primitives.size();
}

}// end namespace ph