#include "Actor/ALight.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/LightSource/LightSource.h"
#include "Core/CoreActor.h"
#include "FileIO/InputPacket.h"

#include <algorithm>
#include <iostream>

namespace ph
{

ALight::ALight() : 
	PhysicalActor(), 
	m_geometry(nullptr), m_material(nullptr), m_lightSource(nullptr)
{

}

ALight::ALight(const std::shared_ptr<LightSource>& lightSource) : 
	PhysicalActor(),
	m_geometry(nullptr), m_material(nullptr), m_lightSource(lightSource)
{

}

ALight::ALight(const ALight& other) : 
	PhysicalActor(other),
	m_geometry(other.m_geometry), m_material(other.m_material), m_lightSource(other.m_lightSource)
{

}

ALight::ALight(const InputPacket& packet) : 
	PhysicalActor(packet),
	m_geometry(nullptr), m_material(nullptr), m_lightSource(nullptr)
{
	m_geometry    = packet.getGeometry("geometry");
	m_material    = packet.getMaterial("material");
	m_lightSource = packet.getLightSource("light-source", "ALight >> parameter light-source not found");
}

ALight::~ALight() = default;

void swap(ALight& first, ALight& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
	swap(first.m_lightSource,                second.m_lightSource);
}

ALight& ALight::operator = (ALight rhs)
{
	swap(*this, rhs);

	return *this;
}

void ALight::genCoreActor(CoreActor* const out_coreActor) const
{
	CoreActor coreActor;

	if(m_geometry && m_material)
	{
		std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();
		metadata->worldToLocal = m_worldToLocal;
		metadata->localToWorld = m_localToWorld;

		std::vector<std::unique_ptr<Primitive>> primitives;
		m_geometry->discretize(&primitives, *metadata);
		m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

		coreActor.primitives        = std::move(primitives);
		coreActor.primitiveMetadata = std::move(metadata);
	}

	if(m_lightSource)
	{
		m_lightSource->buildEmitter(coreActor);
		*out_coreActor = std::move(coreActor);
	}
	else
	{
		std::cerr << "warning: at ALight::genCoreActor(), incomplete data detected" << std::endl;
	}
}

const Geometry* ALight::getGeometry() const
{
	return m_geometry.get();
}

const Material* ALight::getMaterial() const
{
	return m_material.get();
}

const LightSource* ALight::getLightSource() const
{
	return m_lightSource.get();
}

void ALight::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void ALight::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void ALight::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

}// end namespace ph