#include "Actor/AModel.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/CookedActor.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/MotionSource/MotionSource.h"
#include "Core/Quantity/Time.h"

#include <algorithm>
#include <iostream>

namespace ph
{

AModel::AModel() : 
	PhysicalActor(), 
	m_geometry(nullptr), 
	m_material(nullptr), 
	m_motionSource(nullptr)
{

}

AModel::AModel(const std::shared_ptr<Geometry>& geometry, 
               const std::shared_ptr<Material>& material) : 
	PhysicalActor(), 
	m_geometry(geometry), 
	m_material(material), 
	m_motionSource(nullptr)
{

}

AModel::AModel(const AModel& other) : 
	PhysicalActor(other), 
	m_geometry(other.m_geometry), 
	m_material(other.m_material), 
	m_motionSource(other.m_motionSource)
{
	
}

AModel::~AModel() = default;


AModel& AModel::operator = (AModel rhs)
{
	swap(*this, rhs);

	return *this;
}

void AModel::cook(CookedActor* const out_cookedActor) const
{
	CookedActor               cookedActor;
	PrimitiveBuildingMaterial primitiveBuildingMaterial;

	if(m_geometry && m_material)
	{
		//auto localToWorld = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
		//auto worldToLocal = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));

		std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();

		primitiveBuildingMaterial.metadata = metadata.get();
		std::vector<std::unique_ptr<Primitive>> primitives;
		m_geometry->genPrimitive(primitiveBuildingMaterial, primitives);
		m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

		for(auto& primitive : primitives)
		{
			auto localToWorld  = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
			auto worldToLocal  = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));
			auto intersectable = std::make_unique<TransformedIntersectable>(std::move(primitive),
			                                                                std::move(localToWorld), 
			                                                                std::move(worldToLocal));
			
			// HACK

			std::unique_ptr<TransformedIntersectable> motionIntersectable = nullptr;
			if(m_motionSource)
			{
				Time t0;
				Time t1;
				t1.absoluteS = 1;
				t1.relativeS = 1;
				t1.relativeT = 1;
				auto motionLocalToWorld = m_motionSource->genLocalToWorld(t0, t1);
				auto motionWorldToLocal = motionLocalToWorld->genInversed();
				motionIntersectable = std::move(std::make_unique<TransformedIntersectable>(std::move(intersectable),
					std::move(motionLocalToWorld),
					std::move(motionWorldToLocal)));

				//std::cerr << "processing motion source" << std::endl;
			}

			if(!motionIntersectable)
				cookedActor.intersectables.push_back(std::move(intersectable));
			else
				cookedActor.intersectables.push_back(std::move(motionIntersectable));
		}
		cookedActor.primitiveMetadata = std::move(metadata);
	}
	else
	{
		std::cerr << "warning: at AModel::cook(), " 
		          << "incomplete data detected" << std::endl;
	}

	*out_cookedActor = std::move(cookedActor);
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

void swap(AModel& first, AModel& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
	swap(first.m_motionSource,               second.m_motionSource);
}

// command interface

AModel::AModel(const InputPacket& packet) :
	PhysicalActor(packet),
	m_geometry(nullptr), m_material(nullptr), m_motionSource(nullptr)
{
	const DataTreatment requiredDT(EDataImportance::REQUIRED, 
	                               "AModel needs both a Geometry and a Material");
	m_geometry     = packet.get<Geometry>("geometry", requiredDT);
	m_material     = packet.get<Material>("material", requiredDT);
	m_motionSource = packet.get<MotionSource>("motion", DataTreatment::OPTIONAL());
	/*const DataTreatment requiredDT(EDataImportance::REQUIRED, "AModel needs both a Geometry and a Material");
	m_geometry = packet.getGeometry("geometry", requiredDT);
	m_material = packet.getMaterial("material", requiredDT);*/
}

SdlTypeInfo AModel::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "model");
}

ExitStatus AModel::ciExecute(const std::shared_ptr<AModel>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return PhysicalActor::ciExecute(targetResource, functionName, packet);
}

}// end namespace ph