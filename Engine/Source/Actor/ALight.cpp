#include "Actor/ALight.h"
#include "Math/Math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/LightSource/LightSource.h"
#include "Actor/CookedUnit.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/ModelBuilder.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Math/Transform/StaticRigidTransform.h"

#include <algorithm>
#include <iostream>

namespace ph
{

const Logger ALight::logger(LogSender("Actor Light"));

ALight::ALight() : 
	PhysicalActor(), 
	m_geometry(nullptr), m_material(nullptr), m_lightSource(nullptr)
{}

ALight::ALight(const std::shared_ptr<LightSource>& lightSource) : 
	PhysicalActor(),
	m_geometry(nullptr), m_material(nullptr), m_lightSource(lightSource)
{}

ALight::ALight(const ALight& other) : 
	PhysicalActor(other),
	m_geometry(other.m_geometry), m_material(other.m_material), m_lightSource(other.m_lightSource)
{}

ALight::~ALight() = default;


ALight& ALight::operator = (ALight rhs)
{
	swap(*this, rhs);

	return *this;
}

// TODO: simply this method
CookedUnit ALight::cook(CookingContext& context) const
{
	if(!m_lightSource)
	{
		std::cerr << "warning: at ALight::cook(), "
		          << "incomplete data detected" << std::endl;
		return CookedUnit();
	}

	CookedUnit cookedActor;
	if(m_geometry)
	{
		cookedActor = buildGeometricLight(context);
	}
	else
	{
		auto baseLW = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
		auto baseWL = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));

		// TODO: transforms

		EmitterBuildingMaterial emitterBuildingMaterial;
		cookedActor.emitters.push_back(m_lightSource->genEmitter(context, emitterBuildingMaterial));
		cookedActor.transforms.push_back(std::move(baseLW));
		cookedActor.transforms.push_back(std::move(baseWL));
	}

	return cookedActor;
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

CookedUnit ALight::buildGeometricLight(CookingContext& context) const
{
	std::shared_ptr<Material> material = m_material;
	if(!material)
	{
		logger.log(ELogLevel::NOTE_MED, 
		           "material is not specified, using default diffusive material");

		material = std::make_shared<MatteOpaque>();
	}

	std::unique_ptr<RigidTransform> baseLW, baseWL;
	auto sanifiedGeometry = getSanifiedEmitterGeometry(context, &baseLW, &baseWL);
	if(!sanifiedGeometry)
	{
		logger.log(ELogLevel::WARNING_MED,
		           "sanified geometry cannot be made during the process of "
		           "geometric light building; proceed at your own risk");

		sanifiedGeometry = m_geometry;
	}

	CookedUnit cookedActor;

	PrimitiveMetadata* metadata;
	{
		auto primitiveMetadata = std::make_unique<PrimitiveMetadata>();
		metadata = primitiveMetadata.get();
		cookedActor.primitiveMetadatas.push_back(std::move(primitiveMetadata));
	}

	material->populateSurfaceBehavior(context, &(metadata->surfaceBehavior));

	std::vector<std::unique_ptr<Primitive>> primitiveData;
	sanifiedGeometry->genPrimitive(PrimitiveBuildingMaterial(metadata), primitiveData);

	std::vector<const Primitive*> primitives;
	for(auto& primitiveDatum : primitiveData)
	{
		// TODO: baseLW & baseWL may be identity transform if base transform
		// is applied to the geometry, in such case, wrapping primitives with
		// TransformedIntersectable is a total waste
		auto transformedPrimitive = std::make_unique<TransformedPrimitive>(primitiveDatum.get(), 
		                                                                   baseLW.get(), 
		                                                                   baseWL.get());

		primitives.push_back(transformedPrimitive.get());

		context.addBackend(std::move(primitiveDatum));
		cookedActor.intersectables.push_back(std::move(transformedPrimitive));
	}

	EmitterBuildingMaterial emitterBuildingMaterial;
	emitterBuildingMaterial.primitives = primitives;
	auto emitter = m_lightSource->genEmitter(context, emitterBuildingMaterial);
	metadata->surfaceBehavior.setEmitter(emitter.get());

	cookedActor.emitters.push_back(std::move(emitter));

	cookedActor.transforms.push_back(std::move(baseLW));
	cookedActor.transforms.push_back(std::move(baseWL));

	return cookedActor;
}

std::shared_ptr<Geometry> ALight::getSanifiedEmitterGeometry(
	CookingContext& context,
	std::unique_ptr<RigidTransform>* const out_baseLW,
	std::unique_ptr<RigidTransform>* const out_baseWL) const
{
	std::shared_ptr<Geometry> sanifiedGeometry = nullptr;
	*out_baseLW = nullptr;
	*out_baseWL = nullptr;

	auto staticBaseLW = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
	auto staticBaseWL = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.hasScaleEffect())
	{
		sanifiedGeometry = m_geometry->genTransformApplied(*staticBaseLW);
		if(sanifiedGeometry != nullptr)
		{
			// TODO: combine identity transforms...
			*out_baseLW = std::make_unique<StaticRigidTransform>(StaticRigidTransform::makeIdentity());
			*out_baseWL = std::make_unique<StaticRigidTransform>(StaticRigidTransform::makeIdentity());
		}
		else
		{
			std::cerr << "warning: at ALight::getSanifiedEmitterGeometry(), "
			          << "scale detected and has failed to apply it to the geometry; "
			          << "scaling on light with attached geometry may have unexpected "
			          << "behaviors such as miscalculated primitive surface area, which "
			          << "can cause severe rendering artifacts" << std::endl;
		}
	}
	else
	{
		sanifiedGeometry = m_geometry;
		*out_baseLW = std::make_unique<StaticRigidTransform>(StaticRigidTransform::makeForward(m_localToWorld));
		*out_baseWL = std::make_unique<StaticRigidTransform>(StaticRigidTransform::makeForward(m_localToWorld));
	}

	return sanifiedGeometry;
}

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

SdlTypeInfo ALight::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "light");
}

void ALight::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ALight>(ciLoad);
	cmdRegister.setLoader(loader);

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<ALight>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<ALight>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<ALight>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

std::unique_ptr<ALight> ALight::ciLoad(const InputPacket& packet)
{
	const DataTreatment requiredData(EDataImportance::REQUIRED, 
	                                 "ALight requires at least a LightSource");
	const auto lightSource = packet.get<LightSource>("light-source", requiredData);
	const auto geometry    = packet.get<Geometry>("geometry");
	const auto material    = packet.get<Material>("material");

	std::unique_ptr<ALight> light = std::make_unique<ALight>(lightSource);
	light->setGeometry(geometry);
	light->setMaterial(material);
	return light;
}

}// end namespace ph