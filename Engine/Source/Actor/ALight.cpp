#include "Actor/ALight.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/LightSource/LightSource.h"
#include "Actor/CookedUnit.h"
#include "DataIO/SDL/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/ModelBuilder.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Actor/CookingContext.h"

#include <algorithm>
#include <iostream>

namespace ph
{

const Logger ALight::logger(LogSender("Actor Light"));

ALight::ALight() : 
	PhysicalActor(), 
	m_lightSource(nullptr)
{}

ALight::ALight(const std::shared_ptr<LightSource>& lightSource) : 
	PhysicalActor(),
	m_lightSource(lightSource)
{}

ALight::ALight(const ALight& other) : 
	PhysicalActor(other),
	m_lightSource(other.m_lightSource)
{}

ALight& ALight::operator = (ALight rhs)
{
	swap(*this, rhs);

	return *this;
}

CookedUnit ALight::cook(CookingContext& context) const
{
	if(!m_lightSource)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"incomplete data detected, this light is ignored");
		return CookedUnit();
	}

	PH_ASSERT(m_lightSource);
	std::shared_ptr<Geometry> geometry = m_lightSource->genGeometry(context);

	CookedUnit cookedActor;
	if(geometry)
	{
		std::shared_ptr<Material> material = m_lightSource->genMaterial(context);
		cookedActor = buildGeometricLight(context, geometry, material);
	}
	else
	{
		std::unique_ptr<Emitter> emitter = m_lightSource->genEmitter(context, EmitterBuildingMaterial());
		cookedActor.setEmitter(std::move(emitter));
	}

	return cookedActor;
}

const LightSource* ALight::getLightSource() const
{
	return m_lightSource.get();
}

void ALight::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

CookedUnit ALight::buildGeometricLight(
	CookingContext&           context,
	std::shared_ptr<Geometry> geometry,
	std::shared_ptr<Material> material) const
{
	PH_ASSERT(geometry);

	if(!material)
	{
		logger.log(ELogLevel::NOTE_MED, 
			"material is not specified, using default diffusive material");

		material = std::make_shared<MatteOpaque>();
	}

	std::unique_ptr<math::RigidTransform> baseLW, baseWL;
	auto sanifiedGeometry = getSanifiedGeometry(context, geometry, &baseLW, &baseWL);
	if(!sanifiedGeometry)
	{
		logger.log(ELogLevel::WARNING_MED,
			"sanified geometry cannot be made during the process of "
			"geometric light building; proceed at your own risk");

		sanifiedGeometry = geometry;
	}

	CookedUnit cookedActor;

	PrimitiveMetadata* metadata;
	{
		auto primitiveMetadata = std::make_unique<PrimitiveMetadata>();
		metadata = primitiveMetadata.get();
		cookedActor.setPrimitiveMetadata(std::move(primitiveMetadata));
	}

	material->genBehaviors(context, *metadata);

	std::vector<std::unique_ptr<Primitive>> primitiveData;
	sanifiedGeometry->genPrimitive(PrimitiveBuildingMaterial(metadata), primitiveData);

	std::vector<const Primitive*> primitives;
	for(auto& primitiveDatum : primitiveData)
	{
		// TODO: baseLW & baseWL may be identity transform if base transform
		// is applied to the geometry, in such case, wrapping primitives with
		// TransformedIntersectable is a total waste
		auto transformedPrimitive = std::make_unique<TransformedPrimitive>(
			primitiveDatum.get(), 
			baseLW.get(), 
			baseWL.get());

		primitives.push_back(transformedPrimitive.get());

		cookedActor.addBackend(std::move(primitiveDatum));
		cookedActor.addIntersectable(std::move(transformedPrimitive));
	}

	// HACK
	const Primitive* firstPrimitive = primitives.front();

	EmitterBuildingMaterial emitterBuildingMaterial;
	emitterBuildingMaterial.primitives = primitives;
	emitterBuildingMaterial.metadata   = metadata;
	auto emitter = m_lightSource->genEmitter(context, std::move(emitterBuildingMaterial));

	metadata->getSurface().setEmitter(emitter.get());
	cookedActor.setEmitter(std::move(emitter));

	cookedActor.addTransform(std::move(baseLW));
	cookedActor.addTransform(std::move(baseWL));

	return cookedActor;
}

std::shared_ptr<Geometry> ALight::getSanifiedGeometry(
	CookingContext&                        context,
	const std::shared_ptr<Geometry>&       geometry,
	std::unique_ptr<math::RigidTransform>* const out_baseLW,
	std::unique_ptr<math::RigidTransform>* const out_baseWL) const
{
	std::shared_ptr<Geometry> sanifiedGeometry = nullptr;
	*out_baseLW = nullptr;
	*out_baseWL = nullptr;

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.hasScaleEffect())
	{
		const auto baseLW = math::StaticAffineTransform::makeForward(m_localToWorld);

		sanifiedGeometry = geometry->genTransformed(baseLW);
		if(!sanifiedGeometry)
		{
			logger.log(ELogLevel::WARNING_MED,
				"scale detected and has failed to apply it to the geometry; "
				"scaling on light with attached geometry may have unexpected "
				"behaviors such as miscalculated primitive surface area, which "
				"can cause severe rendering artifacts");

			sanifiedGeometry = geometry;
			*out_baseLW = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeForward(m_localToWorld));
			*out_baseWL = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeInverse(m_localToWorld));
		}
		else
		{
			// TODO: combine identity transforms...
			*out_baseLW = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::IDENTITY());
			*out_baseWL = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::IDENTITY());
		}
	}
	else
	{
		sanifiedGeometry = geometry;
		*out_baseLW = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeForward(m_localToWorld));
		*out_baseWL = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeInverse(m_localToWorld));
	}

	return sanifiedGeometry;
}

void swap(ALight& first, ALight& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_lightSource,                second.m_lightSource);
}

// command interface

ALight::ALight(const InputPacket& packet) : 
	PhysicalActor(packet),
	m_lightSource(nullptr)
{
	m_lightSource = packet.getReference<LightSource>(
		"light-source",
		DataTreatment(EDataImportance::REQUIRED, "ALight requires at least a LightSource"));
}

SdlTypeInfo ALight::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "light");
}

void ALight::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<ALight>(packet);
	}));

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

}// end namespace ph
