#include "Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedUIntBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedVertexBuffer.h"
#include "Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/DataIO/Stream/BinaryFileOutputStream.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/exceptions.h>

#include <cstddef>
#include <cstring>
#include <format>

namespace ph
{

namespace
{

inline uint32 load_uint32(const std::byte* const bytes)
{
	uint32 value;
	std::memcpy(&value, bytes, sizeof(uint32));
	return value;
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
			"raw-vert-positions={}, raw-vert-normals={}, raw-vert-uvs={}, vert-position-indices={}, vert-loop-indices={}, tri-mat-ids={}",
			rawVertPositions.size(), rawVertLoopNormals.size(), rawVertLoopUVs.size(), vertPositionIndices.size(), vertLoopIndices.size(), triMatIds.size());
	}

	BinaryFileOutputStream writeStream(path);

	// Write header
	writeStream.writeData<char>(std::format(
		"ply\n"
		"format binary_little_endian 1.0\n"
		"element raw_vert_positions {}\n"
		"property float x\n"
		"property float y\n"
		"property float z\n"
		"element raw_vert_loop_normals {}\n"
		"property float nx\n"
		"property float ny\n"
		"property float nz\n"
		"element raw_vert_loop_uvs {}\n"
		"property float u\n"
		"property float v\n"
		"element position_indices {}\n"
		"property uint pi\n"
		"element loop_indices {}\n"
		"property uint li\n"
		"element mat_ids {}\n"
		"property uint mi\n"
		"end_header\n",
		numPosVerts,
		numLoopVerts,
		numLoopVerts,
		numTris * 3,
		numTris * 3,
		numTris));

	// Write actual data
	writeStream.writeData<float32>(rawVertPositions);
	writeStream.writeData<float32>(rawVertLoopNormals);
	writeStream.writeData<float32>(rawVertLoopUVs);
	writeStream.writeData<uint32>(vertPositionIndices);
	writeStream.writeData<uint32>(vertLoopIndices);
	writeStream.writeData<uint32>(triMatIds);
}

IndexedTriangleBuffer GBlenderPlyPolygonMesh::loadTriangleBuffer(PlyFile& file) const
{
	return loadDirectlyExpandedBlenderTriangleBuffer(file);
}

IndexedTriangleBuffer GBlenderPlyPolygonMesh::loadDirectlyExpandedBlenderTriangleBuffer(PlyFile& file)
{
	constexpr std::size_t positionSize = 3 * sizeof(float32);
	constexpr std::size_t normalSize = 3 * sizeof(float32);
	constexpr std::size_t uvSize = 2 * sizeof(float32);

	const PlyElement& rawPositionElement = *file.findElement("raw_vert_positions");
	const PlyElement& rawLoopNormalElement = *file.findElement("raw_vert_loop_normals");
	const PlyElement& rawLoopUvElement = *file.findElement("raw_vert_loop_uvs");
	const PlyElement& positionIndexElement = *file.findElement("position_indices");
	const PlyElement& loopIndexElement = *file.findElement("loop_indices");

	const std::size_t numLoops = rawLoopNormalElement.numElements;
	const std::size_t numIndices = positionIndexElement.numElements;
	const std::size_t loopNormalBytes = numLoops * normalSize;
	const std::size_t loopUvBytes = numLoops * uvSize;
	const std::size_t indexBytes = numIndices * sizeof(uint32);

	IndexedTriangleBuffer loadedBuffer;
	IndexedVertexBuffer& vertexBuffer = loadedBuffer.getVertexBuffer();
	IndexedUIntBuffer& indexBuffer = loadedBuffer.getIndexBuffer();

	const std::size_t normalOffset = numLoops * positionSize;
	const std::size_t uvOffset = normalOffset + numLoops * normalSize;
	vertexBuffer.declareAttribute(
		EVertexAttribute::Position_0, EVertexElement::Float32, 3, 0, positionSize);
	vertexBuffer.declareAttribute(
		EVertexAttribute::Normal_0, EVertexElement::Float32, 3, normalOffset, normalSize);
	vertexBuffer.declareAttribute(
		EVertexAttribute::TexCoord_0, EVertexElement::Float32, 2, uvOffset, uvSize);
	vertexBuffer.allocate(numLoops);

	// Cook in Blender loop order so split normals/UVs copy linearly and loop indices become
	// the unified index buffer.
	std::byte* const vertexBytes = vertexBuffer.getData();
	std::memcpy(
		vertexBytes + normalOffset,
		rawLoopNormalElement.rawBuffer.data(),
		loopNormalBytes);
	std::memcpy(
		vertexBytes + uvOffset,
		rawLoopUvElement.rawBuffer.data(),
		loopUvBytes);

	indexBuffer.declareUIntFormat<uint32>();
	indexBuffer.allocate(numIndices);
	std::memcpy(
		indexBuffer.getData(),
		loopIndexElement.rawBuffer.data(),
		indexBytes);

	const std::byte* const positionBytes = rawPositionElement.rawBuffer.data();
	const std::byte* const positionIndexBytes = positionIndexElement.rawBuffer.data();
	const std::byte* const loopIndexBytes = loopIndexElement.rawBuffer.data();
	// Expand shared positions into Blender loop-indexed cooked vertices.
	for(std::size_t i = 0; i < numIndices; ++i)
	{
		const auto positionIndex = load_uint32(positionIndexBytes + i * sizeof(uint32));
		const auto loopIndex = load_uint32(loopIndexBytes + i * sizeof(uint32));
		std::memcpy(
			vertexBytes + loopIndex * positionSize,
			positionBytes + positionIndex * positionSize,
			positionSize);
	}

	return loadedBuffer;
}

void GBlenderPlyPolygonMesh::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	IndexedTriangleBuffer* triangleBuffer = ctx.getResources().makeTriangleBuffer();

	PlyFile file(getPlyFile().getPath());
	*triangleBuffer = loadTriangleBuffer(file);

	auto* kdTreeMesh = ctx.getResources().makeIntersectable<TPIndexedKdTreeTriangleMesh<uint32>>(
		triangleBuffer);

	out_geometry.primitives.push_back(kdTreeMesh);
	out_geometry.triangleView = triangleBuffer;
}

}// end namespace ph
