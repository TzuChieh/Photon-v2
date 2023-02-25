#include "Actor/ALight.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/MatteOpaque.h"
#include "World/Foundation/CookedUnit.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/ModelBuilder.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Common/logging.h"

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ALight, Actor);

PreCookReport ALight::preCook(CookingContext& ctx)
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.hasScaleEffect())
	{
		report.setBaseTransforms(nullptr, nullptr);
	}
	else
	{
		auto localToWorld = ctx.getCooked()->makeTransform<math::StaticRigidTransform>(
			math::StaticRigidTransform::makeForward(m_localToWorld));
		auto worldToLocal = ctx.getCooked()->makeTransform<math::StaticRigidTransform>(
			math::StaticRigidTransform::makeInverse(m_localToWorld));

		report.setBaseTransforms(localToWorld, worldToLocal);
	}

	return report;
}

CookedUnit ALight::cook(CookingContext& ctx, const PreCookReport& report)
{
	if(!m_lightSource)
	{
		PH_LOG_WARNING(ALight, "incomplete data detected, this light is ignored");
		return CookedUnit();
	}

	PH_ASSERT(m_lightSource);
	std::shared_ptr<Geometry> geometry = m_lightSource->genGeometry(ctx);

	CookedUnit cookedActor;
	if(geometry)
	{
		std::shared_ptr<Material> material = m_lightSource->genMaterial(ctx);
		cookedActor = buildGeometricLight(ctx, geometry, material);
	}
	else
	{
		std::unique_ptr<Emitter> emitter = m_lightSource->genEmitter(ctx, EmitterBuildingMaterial());
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
	CookingContext& ctx,
	std::shared_ptr<Geometry> geometry,
	std::shared_ptr<Material> material) const
{
	PH_ASSERT(geometry);

	if(!material)
	{
		PH_LOG(ALight, "material is not specified, using default diffusive material");

		material = std::make_shared<MatteOpaque>();
	}

	std::unique_ptr<math::RigidTransform> baseLW, baseWL;
	auto sanifiedGeometry = getSanifiedGeometry(ctx, geometry, &baseLW, &baseWL);
	if(!sanifiedGeometry)
	{
		PH_LOG_WARNING(ALight,
			"sanified geometry cannot be made during the process of "
			"geometric light building; proceed at your own risk");

		sanifiedGeometry = geometry;
	}

	PrimitiveMetadata* metadata = ctx.getCooked()->makeMetadata();
	material->genBehaviors(ctx, *metadata);

	std::vector<std::unique_ptr<Primitive>> primitiveData;
	sanifiedGeometry->genPrimitive(PrimitiveBuildingMaterial(metadata), primitiveData);

	CookedUnit cookedActor;
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

	EmitterBuildingMaterial emitterBuildingMaterial;
	emitterBuildingMaterial.primitives = primitives;
	emitterBuildingMaterial.metadata   = metadata;
	auto emitter = m_lightSource->genEmitter(ctx, std::move(emitterBuildingMaterial));

	metadata->getSurface().setEmitter(emitter.get());
	cookedActor.setEmitter(std::move(emitter));

	cookedActor.addTransform(std::move(baseLW));
	cookedActor.addTransform(std::move(baseWL));

	return cookedActor;
}

std::shared_ptr<Geometry> ALight::getSanifiedGeometry(
	CookingContext&                        ctx,
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
			PH_LOG_WARNING(ALight,
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

}// end namespace ph
