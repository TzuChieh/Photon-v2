#include "Actor/AModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/CookedUnit.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/CookingContext.h"

#include <algorithm>
#include <iostream>

namespace ph
{

AModel::AModel() : 
	PhysicalActor(), 
	m_geometry(nullptr), 
	m_material(nullptr), 
	m_motionSource(nullptr)
{}

AModel::AModel(const std::shared_ptr<Geometry>& geometry, 
               const std::shared_ptr<Material>& material) : 
	PhysicalActor(), 
	m_geometry(geometry), 
	m_material(material), 
	m_motionSource(nullptr)
{}

AModel::AModel(const AModel& other) : 
	PhysicalActor(other), 
	m_geometry(other.m_geometry), 
	m_material(other.m_material), 
	m_motionSource(other.m_motionSource)
{}

AModel& AModel::operator = (AModel rhs)
{
	swap(*this, rhs);

	return *this;
}

CookedUnit AModel::cook(CookingContext& ctx)
{
	if(!m_geometry || !m_material)
	{
		std::cerr << "warning: at AModel::cook(), " 
		          << "incomplete data detected" << std::endl;
		return CookedUnit();
	}

	ModelBuilder builder(ctx);
	
	auto metadata = std::make_unique<PrimitiveMetadata>();

	PrimitiveBuildingMaterial primitiveBuildingMatl(metadata.get());

	std::vector<std::unique_ptr<Primitive>> primitives;
	m_geometry->genPrimitive(primitiveBuildingMatl, primitives);
	for(auto& primitive : primitives)
	{
		builder.addIntersectable(std::move(primitive));
	}

	m_material->genBehaviors(ctx, *metadata);

	builder.addPrimitiveMetadata(std::move(metadata));

	auto baseLW = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeForward(m_localToWorld));
	auto baseWL = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeInverse(m_localToWorld));
	builder.transform(std::move(baseLW), std::move(baseWL));
	
	if(m_motionSource)
	{
		// HACK
		Time t0;
		Time t1;
		t1.absoluteS = 1;
		t1.relativeS = 1;
		t1.relativeT = 1;

		auto motionLW = m_motionSource->genLocalToWorld(t0, t1);
		auto motionWL = motionLW->genInversed();
		builder.transform(std::move(motionLW), std::move(motionWL));
	}

	return builder.claimBuildResult();
}

void AModel::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void AModel::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void AModel::setMotionSource(const std::shared_ptr<MotionSource>& motion)
{
	m_motionSource = motion;
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
	// Enable ADL
	using std::swap;

	// By swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
	swap(first.m_motionSource,               second.m_motionSource);
}

}// end namespace ph
