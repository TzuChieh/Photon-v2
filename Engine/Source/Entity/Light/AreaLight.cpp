#include "Entity/Light/AreaLight.h"
#include "Core/Emitter/EmitterMetadata.h"
#include "Core/Emitter/PrimitiveEmitter.h"

#include <iostream>
#include <vector>

namespace ph
{

AreaLight::~AreaLight() = default;

void AreaLight::buildEmitters(std::vector<std::unique_ptr<Emitter>>* const out_emitters, const EmitterMetadata* const metadata) const
{
	if(!metadata->m_localToWorld || !metadata->m_worldToLocal || !metadata->m_material || !metadata->m_textureMapper)
	{
		std::cerr << "warning: at AreaLight::buildEmitters(), incomplete input detected" << std::endl;
		return;
	}


}

}// end namespace ph