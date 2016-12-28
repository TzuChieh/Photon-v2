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
	m_primitives.push_back(std::move(primitive));
}

void CookedActorStorage::add(std::unique_ptr<PrimitiveMetadata> metadata)
{
	m_primitiveMetadatas.push_back(std::move(metadata));
}

void CookedActorStorage::add(std::unique_ptr<Emitter> emitter)
{
	m_emitters.push_back(std::move(emitter));
}

void CookedActorStorage::add(CoreActor&& coreActor)
{
	add(std::move(coreActor.primitives));

	if(coreActor.primitiveMetadata)
	{
		add(std::move(coreActor.primitiveMetadata));
	}
	
	if(coreActor.emitter)
	{
		add(std::move(coreActor.emitter));
	}
}

void CookedActorStorage::add(std::vector<std::unique_ptr<Primitive>>&& primitives)
{
	m_primitives.insert(m_primitives.end(),
		std::make_move_iterator(primitives.begin()),
		std::make_move_iterator(primitives.end()));
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