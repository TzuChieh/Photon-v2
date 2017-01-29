#include "Actor/AModel.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/CoreActor.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <algorithm>
#include <iostream>

namespace ph
{

AModel::AModel() : 
	PhysicalActor(), 
	m_geometry(nullptr), m_material(nullptr)
{

}

AModel::AModel(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) : 
	PhysicalActor(), 
	m_geometry(geometry), m_material(material)
{

}

AModel::AModel(const AModel& other) : 
	PhysicalActor(other), 
	m_geometry(other.m_geometry), m_material(other.m_material)
{
	
}

AModel::AModel(const InputPacket& packet) : 
	PhysicalActor(packet), 
	m_geometry(nullptr), m_material(nullptr)
{
	m_geometry = packet.getGeometry("geometry", "AModel >> parameter geometry not found");
	m_material = packet.getMaterial("material", "AModel >> parameter material not found");
}

AModel::~AModel() = default;

void swap(AModel& first, AModel& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
}

AModel& AModel::operator = (AModel rhs)
{
	swap(*this, rhs);

	return *this;
}

void AModel::genCoreActor(CoreActor* const out_coreActor) const
{
	PrimitiveBuildingMaterial primitiveBuildingMaterial;

	if(m_geometry && m_material)
	{
		std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();
		metadata->worldToLocal = m_worldToLocal;
		metadata->localToWorld = m_localToWorld;

		primitiveBuildingMaterial.metadata = metadata.get();
		std::vector<std::unique_ptr<Primitive>> primitives;
		m_geometry->discretize(primitiveBuildingMaterial, primitives);
		m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

		out_coreActor->primitives        = std::move(primitives);
		out_coreActor->primitiveMetadata = std::move(metadata);
	}
	else
	{
		std::cerr << "warning: at AModel::genCoreActor(), incomplete data detected" << std::endl;
	}
}

void AModel::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void AModel::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

const Geometry* AModel::getGeometry() const
{
	return m_geometry.get();
}

const Material* AModel::getMaterial() const
{
	return m_material.get();
}

}// end namespace ph