#include "Core/Primitive/PrimitiveStorage.h"

namespace ph
{

void PrimitiveStorage::clear()
{
	m_primitives.clear();
	m_metadataBuffer.clear();
	m_primitives.shrink_to_fit();
	m_metadataBuffer.shrink_to_fit();
}

void PrimitiveStorage::add(std::unique_ptr<Primitive> primitive)
{
	m_primitives.push_back(std::move(primitive));
}

void PrimitiveStorage::add(std::vector<std::unique_ptr<Primitive>>&& primitives)
{
	m_primitives.insert(m_primitives.end(),
	                    std::make_move_iterator(primitives.begin()),
	                    std::make_move_iterator(primitives.end()));
}

void PrimitiveStorage::add(std::unique_ptr<PrimitiveMetadata> metadata)
{
	m_metadataBuffer.push_back(std::move(metadata));
}

typename std::vector<std::unique_ptr<Primitive>>::iterator PrimitiveStorage::begin() noexcept
{
	return m_primitives.begin();
}

typename std::vector<std::unique_ptr<Primitive>>::const_iterator PrimitiveStorage::begin() const noexcept
{
	return m_primitives.begin();
}

typename std::vector<std::unique_ptr<Primitive>>::iterator PrimitiveStorage::end() noexcept
{
	return m_primitives.end();
}

typename std::vector<std::unique_ptr<Primitive>>::const_iterator PrimitiveStorage::end() const noexcept
{
	return m_primitives.end();
}

std::size_t PrimitiveStorage::numPrimitives() const
{
	return m_primitives.size();
}

void PrimitiveStorage::add(PrimitiveStorage&& other)
{
	m_primitives.insert(m_primitives.end(),
	                    std::make_move_iterator(other.m_primitives.begin()), 
	                    std::make_move_iterator(other.m_primitives.end()));
	m_metadataBuffer.insert(m_metadataBuffer.end(), 
	                        std::make_move_iterator(other.m_metadataBuffer.begin()), 
	                        std::make_move_iterator(other.m_metadataBuffer.end()));
}

std::vector<std::unique_ptr<PrimitiveMetadata>>& PrimitiveStorage::getMetadataBuffer()
{
	return m_metadataBuffer;
}

}// end namespace ph