#include "Core/CookedLightStorage.h"

namespace ph
{

void CookedLightStorage::clear()
{
	m_emitters.clear();
	m_emitterMetadataBuffer.clear();
	m_emitters.shrink_to_fit();
	m_emitterMetadataBuffer.shrink_to_fit();
}

void CookedLightStorage::add(std::unique_ptr<Emitter> emitter)
{
	m_emitters.push_back(std::move(emitter));
}

void CookedLightStorage::add(std::unique_ptr<EmitterMetadata> metadata)
{
	m_emitterMetadataBuffer.push_back(std::move(metadata));
}

typename std::vector<std::unique_ptr<Emitter>>::iterator CookedLightStorage::begin() noexcept
{
	return m_emitters.begin();
}

typename std::vector<std::unique_ptr<Emitter>>::const_iterator CookedLightStorage::begin() const noexcept
{
	return m_emitters.begin();
}

typename std::vector<std::unique_ptr<Emitter>>::iterator CookedLightStorage::end() noexcept
{
	return m_emitters.end();
}

typename std::vector<std::unique_ptr<Emitter>>::const_iterator CookedLightStorage::end() const noexcept
{
	return m_emitters.end();
}

std::size_t CookedLightStorage::numEmitters() const
{
	return m_emitters.size();
}

const Emitter& CookedLightStorage::operator [] (const std::size_t index) const
{
	return *(m_emitters[index]);
}

}// end namespace ph