#include "Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedUIntBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.h"
#include "Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/DataIO/Stream/BinaryFileOutputStream.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/exceptions.h>
#include <Common/Utility/string_utils.h>

#include <array>
#include <cstddef>
#include <cstring>
#include <limits>
#include <string_view>
#include <utility>

namespace ph
{

namespace
{

inline constexpr std::array<std::string_view, 4> SLOT_TO_CUSTOM_ATTRIBUTE_PROPERTY_NAME = {
	"custom_0",
	"custom_1",
	"custom_2",
	"custom_3"};

inline uint32 load_uint32(const std::byte* const bytes)
{
	uint32 value;
	std::memcpy(&value, bytes, sizeof(uint32));
	return value;
}

inline TIndexRangeMap<uint64, uint32> load_face_id_to_material_slot_map(PlyFile& file)
{
	const PlyElement& matIdElement = *file.findElement("mat_ids");
	const auto numFaces = static_cast<uint32>(matIdElement.numElements);
	const std::byte* const matIdBytes = matIdElement.rawBuffer.data();

	if(numFaces == 0)
	{
		return TIndexRangeMap<uint64, uint32>();
	}

	const auto materialSlotAt = [matIdBytes](const uint32 faceId)
	{
		return load_uint32(matIdBytes + faceId * sizeof(uint32));
	};

	// `TIndexRangeMap` stores face ranges, which can outnumber unique material IDs.
	uint32 numMaterialSlotRanges = 1;
	uint32 previousMaterialSlot = materialSlotAt(0);
	for(uint32 faceId = 1; faceId < numFaces; ++faceId)
	{
		const uint32 faceMaterialSlot = materialSlotAt(faceId);
		if(faceMaterialSlot != previousMaterialSlot)
		{
			++numMaterialSlotRanges;
		}
		previousMaterialSlot = faceMaterialSlot;
	}

	TIndexRangeMap<uint64, uint32> faceIdToMaterialSlot(numMaterialSlotRanges);
	uint32 currentRangeIndex = 0;
	uint32 rangeMaterialSlot = materialSlotAt(0);
	for(uint32 faceId = 1; faceId < numFaces; ++faceId)
	{
		const uint32 faceMaterialSlot = materialSlotAt(faceId);
		if(faceMaterialSlot != rangeMaterialSlot)
		{
			faceIdToMaterialSlot.setRangeMap(currentRangeIndex, faceId - 1, rangeMaterialSlot);
			++currentRangeIndex;
			rangeMaterialSlot = faceMaterialSlot;
		}
	}
	faceIdToMaterialSlot.setRangeMap(currentRangeIndex, numFaces - 1, rangeMaterialSlot);

	return faceIdToMaterialSlot;
}

}// end anonymous namespace

void GBlenderPlyPolygonMesh::SdlWritePly::operator () () const
{
	const auto numPosVerts = rawVertPositions.size() / 3;
	const auto numLoopVerts = rawVertLoopNormals.size() / 3;
	const auto numTris = vertLoopIndices.size() / 3;

	if(rawVertPositions.size() % 3 != 0 ||
	   rawVertLoopNormals.size() % 3 != 0 ||
	   rawVertLoopUVs.size() % 2 != 0 ||
	   vertLoopIndices.size() % 3 != 0 ||
	   numLoopVerts != rawVertLoopUVs.size() / 2 ||
	   vertPositionIndices.size() != vertLoopIndices.size() ||
	   triMatIds.size() != numTris)
	{
		throw_formatted<InvalidArgumentException>(
			"Inconsistent Blender PLY polygon data sizes: "
			"raw-vert-positions={}, raw-vert-normals={}, raw-vert-uvs={}, "
			"vert-position-indices={}, vert-loop-indices={}, tri-mat-ids={}",
			rawVertPositions.size(),
			rawVertLoopNormals.size(),
			rawVertLoopUVs.size(),
			vertPositionIndices.size(),
			vertLoopIndices.size(),
			triMatIds.size());
	}

	// Gather only the custom slots that contain valid data
	const std::array<const std::vector<float32>*, 4> slotToTriCustoms = {
		&triCustom0, &triCustom1, &triCustom2, &triCustom3};
	std::array<std::size_t, 4> specifiedCustomSlots;
	std::size_t numSpecifiedCustomSlots = 0;
	for(std::size_t slot = 0; slot < slotToTriCustoms.size(); ++slot)
	{
		const std::vector<float32>& values = *slotToTriCustoms[slot];
		if(values.empty())
		{
			continue;
		}

		if(values.size() != numTris)
		{
			throw_formatted<InvalidArgumentException>(
				"Inconsistent Blender PLY custom attribute size: tri-custom-{}={}, num-tris={}",
				slot, values.size(), numTris);
		}

		specifiedCustomSlots[numSpecifiedCustomSlots++] = slot;
	}

	BinaryFileOutputStream writeStream(path);

	const auto writeSize =
		[&writeStream](const std::size_t value)
		{
			std::array<char, std::numeric_limits<std::size_t>::digits10 + 1> buffer;
			const auto numChars = string_utils::stringify_int(value, buffer.data(), buffer.size());
			writeStream.writeString(std::string_view(buffer.data(), numChars));
		};

	// Write header
	writeStream.writeString(
		"ply\n"
		"format binary_little_endian 1.0\n"
		"element raw_vert_positions "); writeSize(numPosVerts);
	writeStream.writeString(
		"\n"
		"property float x\n"
		"property float y\n"
		"property float z\n"
		"element raw_vert_loop_normals "); writeSize(numLoopVerts);
	writeStream.writeString(
		"\n"
		"property float nx\n"
		"property float ny\n"
		"property float nz\n"
		"element raw_vert_loop_uvs "); writeSize(numLoopVerts);
	writeStream.writeString(
		"\n"
		"property float u\n"
		"property float v\n"
		"element position_indices "); writeSize(numTris * 3);
	writeStream.writeString(
		"\n"
		"property uint pi\n"
		"element loop_indices "); writeSize(numTris * 3);
	writeStream.writeString(
		"\n"
		"property uint li\n"
		"element mat_ids "); writeSize(numTris);
	writeStream.writeString(
		"\n"
		"property uint mi\n");

	// Potentially declare custom attributes
	if(numSpecifiedCustomSlots > 0)
	{
		writeStream.writeString("element tri_customs "); writeSize(numTris);
		writeStream.writeString("\n");
		for(std::size_t i = 0; i < numSpecifiedCustomSlots; ++i)
		{
			const std::size_t slot = specifiedCustomSlots[i];
			writeStream.writeString("property float ");
			writeStream.writeString(SLOT_TO_CUSTOM_ATTRIBUTE_PROPERTY_NAME[slot]);
			writeStream.writeString("\n");
		}
	}

	writeStream.writeString("end_header\n");

	// Write actual data
	writeStream.writeData<float32>(rawVertPositions);
	writeStream.writeData<float32>(rawVertLoopNormals);
	writeStream.writeData<float32>(rawVertLoopUVs);
	writeStream.writeData<uint32>(vertPositionIndices);
	writeStream.writeData<uint32>(vertLoopIndices);
	writeStream.writeData<uint32>(triMatIds);

	// Write optional custom attributes
	if(numSpecifiedCustomSlots > 0)
	{
		std::vector<float32> interleavedCustomAttributes;
		interleavedCustomAttributes.reserve(numTris * numSpecifiedCustomSlots);
		for(std::size_t triIndex = 0; triIndex < numTris; ++triIndex)
		{
			for(std::size_t i = 0; i < numSpecifiedCustomSlots; ++i)
			{
				const std::size_t slot = specifiedCustomSlots[i];
				interleavedCustomAttributes.push_back((*slotToTriCustoms[slot])[triIndex]);
			}
		}
		writeStream.writeData<float32>(interleavedCustomAttributes);
	}
}

IndexedTriangleBuffer GBlenderPlyPolygonMesh::loadTriangleBuffer(
	PlyFile& file,
	const StaticAffineTransform* const bakedTransform) const
{
	return loadDirectlyExpandedBlenderTriangleBuffer(file, bakedTransform);
}

IndexedTriangleBuffer GBlenderPlyPolygonMesh::loadDirectlyExpandedBlenderTriangleBuffer(
	PlyFile& file,
	const StaticAffineTransform* const bakedTransform)
{
	constexpr std::size_t positionSize = 3 * sizeof(float32);
	constexpr std::size_t normalSize = 3 * sizeof(float32);
	constexpr std::size_t uvSize = 2 * sizeof(float32);

	const PlyElement& rawPositionElement = *file.findElement("raw_vert_positions");
	const PlyElement& rawLoopNormalElement = *file.findElement("raw_vert_loop_normals");
	const PlyElement& rawLoopUvElement = *file.findElement("raw_vert_loop_uvs");
	const PlyElement& positionIndexElement = *file.findElement("position_indices");
	const PlyElement& loopIndexElement = *file.findElement("loop_indices");
	PlyElement* const triCustomElement = file.findElement("tri_customs");

	const std::size_t numIndices = positionIndexElement.numElements;
	const std::size_t numTris = numIndices / 3;
	const std::size_t numLoops = rawLoopNormalElement.numElements;
	const std::size_t loopNormalBytes = numLoops * normalSize;
	const std::size_t loopUvBytes = numLoops * uvSize;
	const std::size_t indexBytes = numIndices * sizeof(uint32);

	IndexedTriangleBuffer loadedBuffer;
	IndexedAttributeBuffer& attributeBuffer = loadedBuffer.getAttributeBuffer();
	IndexedUIntBuffer& indexBuffer = loadedBuffer.getIndexBuffer();

	const std::size_t normalOffset = numLoops * positionSize;
	const std::size_t uvOffset = normalOffset + loopNormalBytes;
	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::Position_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		3,
		0,
		positionSize);
	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::Normal_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		3,
		normalOffset,
		normalSize);
	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::TexCoord_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		2,
		uvOffset,
		uvSize);

	// Potentially declare custom attributes
	const std::size_t customAttributeStorageOffset = uvOffset + loopUvBytes;
	if(triCustomElement)
	{
		constexpr std::array<EPrimitiveAttribute, 4> slotToCustomAttribute = {
			EPrimitiveAttribute::Custom_0,
			EPrimitiveAttribute::Custom_1,
			EPrimitiveAttribute::Custom_2,
			EPrimitiveAttribute::Custom_3};

		PH_ASSERT_EQ(triCustomElement->numElements, numTris);

		for(std::size_t slot = 0; slot < slotToCustomAttribute.size(); ++slot)
		{
			PlyProperty* const customProperty = triCustomElement->findProperty(SLOT_TO_CUSTOM_ATTRIBUTE_PROPERTY_NAME[slot]);
			if(customProperty)
			{
				attributeBuffer.declareAttribute(
					slotToCustomAttribute[slot],
					EAttributeDomain::Face,
					EAttributeElement::Float32,
					1,
					customAttributeStorageOffset + customProperty->strideOffset,
					triCustomElement->strideSize);
			}
		}
	}

	auto attributeWriter = attributeBuffer.allocate(numLoops, numTris);

	// Cook in Blender loop order so split normals/UVs copy linearly
	// and loop indices become the unified index buffer.
	attributeWriter.setData(
		rawLoopNormalElement.rawBuffer.data(),
		loopNormalBytes,
		normalOffset);
	attributeWriter.setData(
		rawLoopUvElement.rawBuffer.data(),
		loopUvBytes,
		uvOffset);

	// Write optional custom attributes
	if(triCustomElement)
	{
		attributeWriter.setData(
			triCustomElement->rawBuffer.data(),
			triCustomElement->rawBuffer.size(),
			customAttributeStorageOffset);
	}

	indexBuffer.declareUIntFormat<uint32>();
	indexBuffer.allocate(numIndices);
	indexBuffer.setUInts(loopIndexElement.rawBuffer.data(), indexBytes);

	const std::byte* const positionBytes = rawPositionElement.rawBuffer.data();
	const std::byte* const positionIndexBytes = positionIndexElement.rawBuffer.data();
	const std::byte* const loopIndexBytes = loopIndexElement.rawBuffer.data();
	// Blender positions have their own indices. Cooked vertex attributes all use loop indices,
	// so here we expand positions into the loop-indexed domain.
	for(std::size_t i = 0; i < numIndices; ++i)
	{
		// Read position using position index buffer and put it in loop-indexed address
		const auto positionIndex = load_uint32(positionIndexBytes + i * sizeof(uint32));
		const auto loopIndex = load_uint32(loopIndexBytes + i * sizeof(uint32));
		attributeWriter.setData(
			positionBytes + positionIndex * positionSize,
			positionSize,
			loopIndex * positionSize);
	}

	if(bakedTransform)
	{
		applyBakedTransform(attributeBuffer, attributeWriter, *bakedTransform);
	}

	return loadedBuffer;
}

void GBlenderPlyPolygonMesh::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	PlyFile file(getPlyFile().getPath());
	
	IndexedTriangleBuffer triangleBuffer = loadTriangleBuffer(file);
	storeCookedPolygonMesh(ctx, std::move(triangleBuffer), out_geometry);

	out_geometry.faceIdToMetadataSlot = load_face_id_to_material_slot_map(file);
}

void GBlenderPlyPolygonMesh::storeCookedWithBakedTransform(
	const CookingContext& ctx,
	const StaticAffineTransform& transform,
	CookedGeometry& out_geometry) const
{
	PlyFile file(getPlyFile().getPath());

	IndexedTriangleBuffer triangleBuffer = loadTriangleBuffer(file, &transform);
	storeCookedPolygonMesh(ctx, std::move(triangleBuffer), out_geometry);
	out_geometry.isWindingFlipped = transform.isWindingFlipped();
	
	out_geometry.faceIdToMetadataSlot = load_face_id_to_material_slot_map(file);
}

}// end namespace ph
