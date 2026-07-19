#include "Engine/Actor/ABlenderPlyModel.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveBuilder.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/TransientVisualElement.h"

#include <memory>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ABlenderPlyModel, Actor);

PreCookReport ABlenderPlyModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	if(!m_geometry || m_materials.empty())
	{
		throw ActorCookException(
			"Blender PLY model requires geometry and at least one material slot.");
	}

	for(std::size_t slotIndex = 0; slotIndex < m_materials.size(); ++slotIndex)
	{
		if(!m_materials[slotIndex])
		{
			throw ActorCookException(
				"Blender PLY model requires every material slot to reference a material.");
		}
	}

	if(!m_localToWorld.getDecomposed().isIdentity())
	{
		auto localToWorld = ctx.getResources().makeTransform<StaticAffineTransform>(
			m_localToWorld.getForwardStaticAffine());
		auto worldToLocal = ctx.getResources().makeTransform<StaticAffineTransform>(
			m_localToWorld.getInverseStaticAffine());

		report.setBaseTransforms(localToWorld, worldToLocal);
	}

	return report;
}

TransientVisualElement ABlenderPlyModel::cook(
	const CookingContext& ctx,
	const PreCookReport& report) const
{
	const CookedGeometry* const cookedGeometry = ctx.getCooked(m_geometry);
	if(cookedGeometry->primitives.empty())
	{
		return TransientVisualElement();
	}

	// Our default policy is Ng facing is unaffectd by winding change, 
	// so if `isWindingFlipped` is true then that implies a flip, if `m_shouldFlipNg`
	// is specified additionally then they can cancel out
	const bool shouldFlipNg = m_shouldFlipNg != cookedGeometry->isWindingFlipped;

	if(cookedGeometry->faceIdToMetadataSlot.isEmpty())
	{
		throw ActorCookException(
			"Blender PLY model requires geometry with face-ID-to-material-slot mapping.");
	}

	cookedGeometry->faceIdToMetadataSlot.forEachEntry(
		[this](uint64 /* maxFaceId */, const uint32 materialSlot)
		{
			if(materialSlot >= m_materials.size())
			{
				throw ActorCookException(
					"Blender PLY model geometry references a material slot not provided by the actor.");
			}
		});

	const auto numMetadataSlots = static_cast<uint32>(m_materials.size());
	auto metadatas = std::make_unique<const PrimitiveMetadata*[]>(numMetadataSlots);
	for(std::size_t slotIndex = 0; slotIndex < m_materials.size(); ++slotIndex)
	{
		const std::shared_ptr<Material>& material = m_materials[slotIndex];
		const CookedMaterial* cookedMaterial = ctx.getCooked(material);
		PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();
		metadata->surface().setOptics(cookedMaterial->surfaceOptics);

		if(material->getOverlapPriority() > 0)
		{
			const VolumeOptics* interiorOptics = nullptr;
			const VolumeOptics* exteriorOptics = nullptr;
			cookedMaterial->findFirstCompatibleOptics(&interiorOptics, &exteriorOptics);

			metadata->interior().setOptics(interiorOptics);
			metadata->exterior().setOptics(exteriorOptics);
			metadata->setInteriorPriority(material->getOverlapPriority());
		}

		metadatas[slotIndex] = metadata;
	}

	const auto* const localToWorld = report.getBaseLocalToWorld();
	const auto* const worldToLocal = report.getBaseWorldToLocal();

	TransientVisualElement result;
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto copiedMetadatas = std::make_unique<const PrimitiveMetadata*[]>(numMetadataSlots);
		for(uint32 slotIndex = 0; slotIndex < numMetadataSlots; ++slotIndex)
		{
			copiedMetadatas[slotIndex] = metadatas[slotIndex];
		}

		auto primitiveBuilder =
			PrimitiveBuilder::referencing(primitive)
				.injectMetadataArray(
					std::move(copiedMetadatas),
					numMetadataSlots,
					&cookedGeometry->faceIdToMetadataSlot);
		// Have transform
		if(localToWorld)
		{
			if(shouldFlipNg)
			{
				result.intersectables.push_back(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.transform<true>(localToWorld, worldToLocal).build()));
			}
			else
			{
				result.intersectables.push_back(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.transform(localToWorld, worldToLocal).build()));
			}
		}
		// No transform
		else
		{
			if(shouldFlipNg)
			{
				result.add(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.flipGeometryNormal().build()));
			}
			else
			{
				result.add(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.build()));
			}
		}
	}

	return result;
}

void ABlenderPlyModel::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void ABlenderPlyModel::setMaterials(std::vector<std::shared_ptr<Material>> materials)
{
	m_materials = std::move(materials);
}

void ABlenderPlyModel::setShouldFlipNg(const bool shouldFlipNg)
{
	m_shouldFlipNg = shouldFlipNg;
}

bool ABlenderPlyModel::shouldFlipNg() const
{
	return m_shouldFlipNg;
}

}// end namespace ph
