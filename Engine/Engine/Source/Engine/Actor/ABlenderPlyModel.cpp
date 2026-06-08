#include "Engine/Actor/ABlenderPlyModel.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveBuilder.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/TransformedIntersectable.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/TransientVisualElement.h"

#include <Common/logging.h>

#include <memory>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ABlenderPlyModel, Actor);

namespace
{

auto copy_metadata_refs(
	const std::unique_ptr<const PrimitiveMetadata*[]>& metadatas,
	const uint32 numMetadatas)
{
	auto copiedMetadatas = std::make_unique<const PrimitiveMetadata*[]>(numMetadatas);
	for(uint32 slotIndex = 0; slotIndex < numMetadatas; ++slotIndex)
	{
		copiedMetadatas[slotIndex] = metadatas[slotIndex];
	}

	return copiedMetadatas;
}

}// end namespace

PreCookReport ABlenderPlyModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

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

	const CookedGeometry* const cookedGeometry = m_geometry->createCooked(ctx);
	if(!cookedGeometry || cookedGeometry->primitives.empty())
	{
		return TransientVisualElement();
	}

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
		PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();
		CookedMaterial* cookedMaterial = material->createCooked(ctx);

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

	TransientVisualElement result;
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto* metaPrimitive = ctx.getResources().copyIntersectable(
			PrimitiveBuilder::referencing(primitive)
				.injectMetadataArray(
					copy_metadata_refs(metadatas, numMetadataSlots),
					numMetadataSlots,
					cookedGeometry->faceIdToMetadataSlot)
				.build());

		result.add(metaPrimitive);
	}

	if(!m_localToWorld.getDecomposed().isIdentity())
	{
		result.primitivesView.clear();

		auto localToWorld = report.getBaseLocalToWorld();
		auto worldToLocal = report.getBaseWorldToLocal();

		for(auto& intersectable : result.intersectables)
		{
			auto* transformedIntersectable = ctx.getResources().makeIntersectable<TransformedIntersectable>(
				intersectable, localToWorld, worldToLocal);

			intersectable = transformedIntersectable;
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

}// end namespace ph
