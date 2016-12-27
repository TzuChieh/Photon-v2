#include "Actor/Light/Light.h"
#include "Math/Math.h"
#include "Actor/Model/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Model/Geometry/Geometry.h"
#include "Actor/Model/Material/Material.h"
#include "Actor/Light/LightSource/LightSource.h"
#include "Core/Emitter/EmitterMetadata.h"

#include <algorithm>
#include <iostream>

namespace ph
{

Light::Light() :
	m_model(), m_lightSource(nullptr)
{

}

Light::Light(const std::shared_ptr<LightSource>& lightSource) :
	m_model(), m_lightSource(lightSource)
{

}

Light::Light(const Light& other) :
	m_model(other.m_model), m_lightSource(other.m_lightSource)
{

}

Light::~Light() = default;

void swap(Light& first, Light& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(first.m_model,       second.m_model);
	swap(first.m_lightSource, second.m_lightSource);
}

Light& Light::operator = (Light rhs)
{
	swap(*this, rhs);

	return *this;
}

void Light::cookData(PrimitiveStorage* const out_primitiveStorage, EmitterStorage* const out_emitterStorage) const
{
	if(m_lightSource)
	{
		m_lightSource->buildEmitters(out_primitiveStorage, out_emitterStorage, m_model);
	}
}

void Light::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

Model& Light::getModel()
{
	return m_model;
}

}// end namespace ph