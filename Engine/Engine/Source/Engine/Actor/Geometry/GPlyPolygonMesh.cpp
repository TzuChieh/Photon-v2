#include "Engine/Actor/Geometry/GPlyPolygonMesh.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedVertexBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedUIntBuffer.h"
#include "Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Math/math.h"
#include "Engine/Utility/Timer.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(GPlyPolygonMesh, Geometry);

void GPlyPolygonMesh::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	Timer loadTimer;
	loadTimer.start();

	IndexedTriangleBuffer triangleBuffer = loadStandardTriangleBuffer();

	loadTimer.stop();

	Timer buildTimer;
	buildTimer.start();

	storeCookedPolygonMesh(ctx, std::move(triangleBuffer), out_geometry);

	buildTimer.stop();

	// Log some stats for performance analysis
	if(out_geometry.triangleView)
	{
		PH_LOG(GPlyPolygonMesh, Note,
			"{} buffer stats: {} verts, {} faces ({:.3f} MiB, {:.3f} B per face)", 
			m_plyFile.getIdentifier(),
			out_geometry.triangleView->getVertexBuffer().numVertices(),
			out_geometry.triangleView->numFaces(),
			math::bytes_to_MiB<double>(out_geometry.triangleView->memoryUsage()),
			out_geometry.triangleView->averagePerPolygonMemoryUsage());

		PH_LOG(GPlyPolygonMesh, Note,
			"{} buffer timings: {:.2f} ms loading, {:.2f} ms building accel",
			m_plyFile.getIdentifier(),
			loadTimer.getDeltaMs<double>(),
			buildTimer.getDeltaMs<double>());
	}
}

void GPlyPolygonMesh::storeCookedWithBakedTransform(
	const CookingContext& ctx,
	const StaticAffineTransform& transform,
	CookedGeometry& out_geometry) const
{
	IndexedTriangleBuffer triangleBuffer = loadStandardTriangleBuffer();
	applyBakedTransform(triangleBuffer, transform);
	storeCookedPolygonMesh(ctx, std::move(triangleBuffer), out_geometry);
	out_geometry.isWindingFlipped = transform.isWindingFlipped();
}

void GPlyPolygonMesh::applyBakedTransform(
	IndexedTriangleBuffer& triangleBuffer,
	const StaticAffineTransform& transform)
{
	IndexedVertexBuffer& vertices = triangleBuffer.getVertexBuffer();
	PH_ASSERT(vertices.hasAttribute(EVertexAttribute::Position_0));

	for(std::size_t vi = 0; vi < vertices.numVertices(); ++vi)
	{
		math::Vector3R tPosition;
		transform.transformP(vertices.getAttribute(EVertexAttribute::Position_0, vi), &tPosition);
		vertices.setAttribute(EVertexAttribute::Position_0, vi, tPosition);
	}

	if(vertices.hasAttribute(EVertexAttribute::Normal_0))
	{
		for(std::size_t vi = 0; vi < vertices.numVertices(); ++vi)
		{
			math::Vector3R tNormal;
			transform.transformO(vertices.getAttribute(EVertexAttribute::Normal_0, vi), &tNormal);
			vertices.setAttribute(EVertexAttribute::Normal_0, vi, tNormal.normalizeLocal());
		}
	}
}

void GPlyPolygonMesh::storeCookedPolygonMesh(
	const CookingContext& ctx,
	IndexedTriangleBuffer triangleBuffer,
	CookedGeometry& out_geometry)
{
	IndexedTriangleBuffer* const storedBuffer = ctx.getResources().makeTriangleBuffer(
		std::move(triangleBuffer));

	// TODO: more index types
	// TODO: count tree memory usage
	auto* const kdTreeMesh = ctx.getResources().makeIntersectable<TPIndexedKdTreeTriangleMesh<uint32>>(
		storedBuffer);

	out_geometry.primitives.push_back(kdTreeMesh);
	out_geometry.triangleView = storedBuffer;
}

IndexedTriangleBuffer GPlyPolygonMesh::loadTriangleBuffer(
	PlyFile& file,
	std::string_view vertexElementName,
	std::string_view positionXPropertyName,
	std::string_view positionYPropertyName,
	std::string_view positionZPropertyName,
	std::string_view normalXPropertyName,
	std::string_view normalYPropertyName,
	std::string_view normalZPropertyName,
	std::string_view faceElementName,
	std::string_view vertexIndicesPropertyName) const
{
	PlyElement* vertexElement = file.findElement(vertexElementName);
	if(!vertexElement)
	{
		throw CookException("cannot find PLY vertex element");
	}

	PlyElement* faceElement = file.findElement(faceElementName);
	if(!faceElement)
	{
		throw CookException("cannot find PLY face element");
	}

	IndexedTriangleBuffer loadedBuffer;
	IndexedVertexBuffer& vertexBuffer = loadedBuffer.getVertexBuffer();
	IndexedUIntBuffer& indexBuffer = loadedBuffer.getIndexBuffer();

	// Loading vertices

	auto xValues = vertexElement->propertyValues(vertexElement->findProperty(positionXPropertyName));
	auto yValues = vertexElement->propertyValues(vertexElement->findProperty(positionYPropertyName));
	auto zValues = vertexElement->propertyValues(vertexElement->findProperty(positionZPropertyName));
	auto nxValues = vertexElement->propertyValues(vertexElement->findProperty(normalXPropertyName));
	auto nyValues = vertexElement->propertyValues(vertexElement->findProperty(normalYPropertyName));
	auto nzValues = vertexElement->propertyValues(vertexElement->findProperty(normalZPropertyName));

	const bool hasVertexCoords = xValues && yValues && zValues;
	const bool hasNormals = nxValues && nyValues && nzValues;

	// Not having full x, y, z coordinates is an error
	if(!hasVertexCoords)
	{
		throw CookException("requires x, y, z coordinates for a triangle buffer");
	}

	vertexBuffer.declareAttribute(
		EVertexAttribute::Position_0,
		EVertexElement::Float32,
		3);

	if(hasNormals)
	{
		vertexBuffer.declareAttribute(
			EVertexAttribute::Normal_0,
			EVertexElement::Float32,
			3);
	}

	vertexBuffer.allocate(vertexElement->numElements);
	for(std::size_t vertexIdx = 0; vertexIdx < vertexElement->numElements; ++vertexIdx)
	{
		const math::Vector3D position(
			xValues.get(vertexIdx), 
			yValues.get(vertexIdx), 
			zValues.get(vertexIdx));

		vertexBuffer.setAttribute(EVertexAttribute::Position_0, vertexIdx, math::Vector3R(position));
	}

	if(hasNormals)
	{
		for(std::size_t vertexIdx = 0; vertexIdx < vertexElement->numElements; ++vertexIdx)
		{
			math::Vector3D normal(
				nxValues.get(vertexIdx), 
				nyValues.get(vertexIdx), 
				nzValues.get(vertexIdx));

			// Re-normalize as some mesh may not come in with normalized normals
			normal.normalizeLocal();

			vertexBuffer.setAttribute(EVertexAttribute::Normal_0, vertexIdx, math::Vector3R(normal));
		}
	}

	// Loading indices

	auto vertexIndexProperty = faceElement->findProperty(vertexIndicesPropertyName);
	auto vertexIndexLists = faceElement->listPropertyValues(vertexIndexProperty);
	if(!vertexIndexLists)
	{
		throw CookException("cannot find PLY vertex indices property");
	}
	else if(!vertexIndexLists.isFixedSizeList())
	{
		throw CookException(
			"PLY vertex indices is not a fixed-size property (variable polygon vertices "
			"is not supported)");
	}
	else if(vertexIndexLists.fixedListSize() != 3)
	{
		throw CookException(
			"a PLY vertex index list must have exactly 3 indices for a polygon (triangle)");
	}

	using IndexType = uint32;

	// For now handling only index sizes <= uint32
	PH_ASSERT(
		vertexIndexProperty->dataType == EPlyDataType::Int8 ||
		vertexIndexProperty->dataType == EPlyDataType::UInt8 ||
		vertexIndexProperty->dataType == EPlyDataType::Int16 ||
		vertexIndexProperty->dataType == EPlyDataType::UInt16 ||
		vertexIndexProperty->dataType == EPlyDataType::Int32 ||
		vertexIndexProperty->dataType == EPlyDataType::UInt32);

	indexBuffer.declareUIntFormat<IndexType>();
	indexBuffer.allocate(vertexIndexLists.size() * 3);
	for(std::size_t faceIdx = 0; faceIdx < vertexIndexLists.size(); ++faceIdx)
	{
		indexBuffer.setUInt(3 * faceIdx + 0, static_cast<IndexType>(vertexIndexLists.get(faceIdx, 0)));
		indexBuffer.setUInt(3 * faceIdx + 1, static_cast<IndexType>(vertexIndexLists.get(faceIdx, 1)));
		indexBuffer.setUInt(3 * faceIdx + 2, static_cast<IndexType>(vertexIndexLists.get(faceIdx, 2)));
	}

	return loadedBuffer;
}

IndexedTriangleBuffer GPlyPolygonMesh::loadStandardTriangleBuffer() const
{
	PH_LOG(GPlyPolygonMesh, Note, "loading standard file {}", m_plyFile);

	PlyFile file(m_plyFile.getPath());
	return loadTriangleBuffer(
		file,
		"vertex",
		"x",
		"y",
		"z",
		"nx",
		"ny",
		"nz",
		"face",
		"vertex_indices");
}

}// end namespace ph
