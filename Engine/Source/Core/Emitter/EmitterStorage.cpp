#include "Core/Emitter/EmitterStorage.h"

namespace ph
{

void EmitterStorage::clear()
{
	m_emitters.clear();
	m_emitterMetadataBuffer.clear();
	m_emitters.shrink_to_fit();
	m_emitterMetadataBuffer.shrink_to_fit();

	m_primitiveStorage.clear();
}

void EmitterStorage::add(std::unique_ptr<Emitter> emitter)
{
	m_emitters.push_back(std::move(emitter));
}

void EmitterStorage::add(std::unique_ptr<EmitterMetadata> metadata)
{
	m_emitterMetadataBuffer.push_back(std::move(metadata));
}

void EmitterStorage::add(PrimitiveStorage&& primitives)
{
	m_primitiveStorage.add(std::move(primitives));
}

typename std::vector<std::unique_ptr<Emitter>>::iterator EmitterStorage::begin() noexcept
{
	return m_emitters.begin();
}

typename std::vector<std::unique_ptr<Emitter>>::const_iterator EmitterStorage::begin() const noexcept
{
	return m_emitters.begin();
}

typename std::vector<std::unique_ptr<Emitter>>::iterator EmitterStorage::end() noexcept
{
	return m_emitters.end();
}

typename std::vector<std::unique_ptr<Emitter>>::const_iterator EmitterStorage::end() const noexcept
{
	return m_emitters.end();
}

std::size_t EmitterStorage::numEmitters() const
{
	return m_emitters.size();
}

const Emitter& EmitterStorage::operator [] (const std::size_t index) const
{
	return *(m_emitters[index]);
}

}// end namespace ph