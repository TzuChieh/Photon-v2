#include "Actor/ALight.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/LightSource/LightSource.h"
#include "Core/CoreActor.h"

#include <algorithm>
#include <iostream>

namespace ph
{

ALight::ALight() :
	m_model(), m_lightSource(nullptr)
{

}

ALight::ALight(const std::shared_ptr<LightSource>& lightSource) :
	m_model(), m_lightSource(lightSource)
{

}

ALight::ALight(const ALight& other) :
	m_model(other.m_model), m_lightSource(other.m_lightSource)
{

}

ALight::~ALight() = default;

void swap(ALight& first, ALight& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(first.m_model,       second.m_model);
	swap(first.m_lightSource, second.m_lightSource);
}

ALight& ALight::operator = (ALight rhs)
{
	swap(*this, rhs);

	return *this;
}

void ALight::genCoreActor(CoreActor* const out_coreActor) const
{
	if(m_lightSource)
	{
		CoreActor coreActor;
		m_model.genCoreActor(&coreActor);
		m_lightSource->buildEmitter(coreActor);

		*out_coreActor = std::move(coreActor);
	}
}

void ALight::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

AModel& ALight::getModel()
{
	return m_model;
}

}// end namespace ph