#include "World/LightStorage.h"
#include "Emitter/Emitter.h"

namespace ph
{

LightStorage::LightStorage()
{

}

void LightStorage::update()
{

}

void LightStorage::addEmitter(const std::shared_ptr<Emitter>& emitter)
{
	m_emitters.push_back(emitter);
}

}// end namespace ph