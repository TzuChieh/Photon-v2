#include "Actor/AModel.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/CookedUnit.h"
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

CookedUnit AModel::cook(CookingContext& context) const
{
	CookedUnit               cookedActor;
	PrimitiveBuildingMaterial primitiveBuildingMaterial;

	if(!m_geometry || !m_material)
	{
		std::cerr << "warning: at AModel::cook(), " 
		          << "incomplete data detected" << std::endl;
		return cookedActor;
	}

	std::vector<std::unique_ptr<Primitive>> primitives;
	auto baseLW   = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
	auto baseWL   = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));
	auto metadata = std::make_unique<PrimitiveMetadata>();

	std::unique_ptr<Transform> motionLW;
	std::unique_ptr<Transform> motionWL;
	if(m_motionSource)
	{
		// HACK
		Time t0;
		Time t1;
		t1.absoluteS = 1;
		t1.relativeS = 1;
		t1.relativeT = 1;

		motionLW = m_motionSource->genLocalToWorld(t0, t1);
		motionWL = motionLW->genInversed();
	}

	primitiveBuildingMaterial.metadata = metadata.get();
	m_geometry->genPrimitive(primitiveBuildingMaterial, primitives);
	m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));
	cookedActor.primitiveMetadata = std::move(metadata);

	// TODO: use nested intersector when > 1 primitives are generated

	for(auto& primitive : primitives)
	{
		auto tIsable = std::make_unique<TransformedIntersectable>(primitive.get(),
		                                                          baseLW.get(),
		                                                          baseWL.get());
		if(!m_motionSource)
		{
			cookedActor.intersectables.push_back(std::move(tIsable));
		}
		else
		{
			auto motionIsable = std::make_unique<TransformedIntersectable>(tIsable.get(),
			                                                               motionLW.get(),
			                                                               motionWL.get());
			context.addBackend(std::move(tIsable));
			cookedActor.intersectables.push_back(std::move(motionIsable));
		}

		context.addBackend(std::move(primitive));
	}

	cookedActor.transforms.push_back(std::move(baseLW));
	cookedActor.transforms.push_back(std::move(baseWL));
	if(m_motionSource)
	{
		cookedActor.transforms.push_back(std::move(motionLW));
		cookedActor.transforms.push_back(std::move(motionWL));
	}

	return cookedActor;
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
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
	swap(first.m_motionSource,               second.m_motionSource);
}

// command interface

SdlTypeInfo AModel::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "model");
}

void AModel::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AModel>(ciLoad);
	cmdRegister.setLoader(loader);

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<AModel>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<AModel>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<AModel>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

std::unique_ptr<AModel> AModel::ciLoad(const InputPacket& packet)
{
	const DataTreatment requiredDT(EDataImportance::REQUIRED, 
	                               "AModel needs both a Geometry and a Material");
	const auto& geometry     = packet.get<Geometry>("geometry", requiredDT);
	const auto& material     = packet.get<Material>("material", requiredDT);
	const auto& motionSource = packet.get<MotionSource>("motion", DataTreatment::OPTIONAL());

	std::unique_ptr<AModel> model = std::make_unique<AModel>(geometry, material);
	model->setMotionSource(motionSource);
	return model;
}

}// end namespace ph