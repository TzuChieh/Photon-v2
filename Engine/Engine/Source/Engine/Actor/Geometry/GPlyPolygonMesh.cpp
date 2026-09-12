#include "Engine/Actor/Geometry/GPlyPolygonMesh.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedUIntBuffer.h"
#include "Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Math/math.h"
#include "Engine/Utility/Timer.h"

#include <Common/logging.h>

#include <cmath>
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
	IndexedTriangleBuffer triangleBuffer = loadStandardTriangleBuffer(&transform);
	storeCookedPolygonMesh(ctx, std::move(triangleBuffer), out_geometry);
	out_geometry.isWindingFlipped = transform.isWindingFlipped();
}

void GPlyPolygonMesh::applyBakedTransform(
	const IndexedAttributeBuffer& srcAttributes,
	IndexedAttributeBufferWriter& dstAttributes,
	const StaticAffineTransform& transform)
{
	PH_ASSERT(srcAttributes.hasAttribute(EPrimitiveAttribute::Position_0));

	for(std::size_t vi = 0; vi < dstAttributes.numVertices(); ++vi)
	{
		math::Vector3R tPosition;
		transform.transformP(srcAttributes.getAttribute(EPrimitiveAttribute::Position_0, vi), &tPosition);
		dstAttributes.setAttribute(EPrimitiveAttribute::Position_0, vi, tPosition);
	}

	if(srcAttributes.hasAttribute(EPrimitiveAttribute::Normal_0))
	{
		for(std::size_t vi = 0; vi < dstAttributes.numVertices(); ++vi)
		{
			math::Vector3R tNormal;
			transform.transformO(srcAttributes.getAttribute(EPrimitiveAttribute::Normal_0, vi), &tNormal);
			dstAttributes.setAttribute(EPrimitiveAttribute::Normal_0, vi, tNormal.normalizeLocal());
		}
	}

	if(srcAttributes.hasAttribute(EPrimitiveAttribute::Tangent_0))
	{
		for(std::size_t vi = 0; vi < dstAttributes.numVertices(); ++vi)
		{
			math::Vector3R tTangent;
			transform.transformV(srcAttributes.getAttribute(EPrimitiveAttribute::Tangent_0, vi), &tTangent);
			dstAttributes.setAttribute(EPrimitiveAttribute::Tangent_0, vi, tTangent.normalizeLocal());
		}
	}

	if(srcAttributes.hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0))
	{
		for(std::size_t vi = 0; vi < dstAttributes.numVertices(); ++vi)
		{
			uint32 tangentSignBit;
			const math::Vector3R tangent = srcAttributes.getAttribute(
				EPrimitiveAttribute::MikkTSpaceTangent_0,
				vi,
				&tangentSignBit);
			PH_ASSERT_LE(tangentSignBit, 1);

			// Tangent sign bit is effectively changing handedness.
			// If winding is already flipped, it can cancel/produce one sign bit, too.
			if(transform.isWindingFlipped())
			{
				tangentSignBit ^= 1;
			}

			math::Vector3R tTangent;
			transform.transformV(tangent, &tTangent);
			dstAttributes.setAttribute(
				EPrimitiveAttribute::MikkTSpaceTangent_0,
				vi,
				tTangent.normalizeLocal(),
				&tangentSignBit);
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
	std::string_view tangentXPropertyName,
	std::string_view tangentYPropertyName,
	std::string_view tangentZPropertyName,
	std::string_view tangentSignPropertyName,
	std::string_view faceElementName,
	std::string_view vertexIndicesPropertyName,
	const StaticAffineTransform* const bakedTransform) const
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
	IndexedAttributeBuffer& attributeBuffer = loadedBuffer.getAttributeBuffer();
	IndexedUIntBuffer& indexBuffer = loadedBuffer.getIndexBuffer();

	// Loading vertices

	auto xValues = vertexElement->propertyValues(vertexElement->findProperty(positionXPropertyName));
	auto yValues = vertexElement->propertyValues(vertexElement->findProperty(positionYPropertyName));
	auto zValues = vertexElement->propertyValues(vertexElement->findProperty(positionZPropertyName));
	auto nxValues = vertexElement->propertyValues(vertexElement->findProperty(normalXPropertyName));
	auto nyValues = vertexElement->propertyValues(vertexElement->findProperty(normalYPropertyName));
	auto nzValues = vertexElement->propertyValues(vertexElement->findProperty(normalZPropertyName));
	auto txValues = vertexElement->propertyValues(vertexElement->findProperty(tangentXPropertyName));
	auto tyValues = vertexElement->propertyValues(vertexElement->findProperty(tangentYPropertyName));
	auto tzValues = vertexElement->propertyValues(vertexElement->findProperty(tangentZPropertyName));
	auto twValues = vertexElement->propertyValues(vertexElement->findProperty(tangentSignPropertyName));

	const bool hasVertexCoords = xValues && yValues && zValues;
	const bool hasNormals = nxValues && nyValues && nzValues;
	const bool hasAnyTangent = txValues || tyValues || tzValues || twValues;
	const bool hasTangents = txValues && tyValues && tzValues;
	const bool hasTangentSign = twValues;

	// Not having full x, y, z coordinates is an error
	if(!hasVertexCoords)
	{
		throw CookException("requires x, y, z coordinates for a triangle buffer");
	}

	if(hasAnyTangent && !hasTangents)
	{
		throw CookException("requires a complete tx, ty, tz property set");
	}

	if(hasTangents && !hasNormals)
	{
		throw CookException("requires vertex normals when tangents are specified");
	}

	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::Position_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		3);

	if(hasNormals)
	{
		attributeBuffer.declareAttribute(
			EPrimitiveAttribute::Normal_0,
			EAttributeDomain::Vertex,
			EAttributeElement::Float32,
			3);
	}

	// Baked reflections require a sign even when the source tangent has none.
	const bool shouldStoreTangentSign =
		hasTangentSign || (bakedTransform && bakedTransform->isWindingFlipped());
	const EPrimitiveAttribute tangentAttribute = shouldStoreTangentSign
		? EPrimitiveAttribute::MikkTSpaceTangent_0
		: EPrimitiveAttribute::Tangent_0;
	if(hasTangents)
	{
		const EAttributeElement tangentElement = shouldStoreTangentSign
			? EAttributeElement::OctahedralUnitVec3_31_CustomBits_1
			: EAttributeElement::OctahedralUnitVec3_32;
		attributeBuffer.declareAttribute(
			tangentAttribute,
			EAttributeDomain::Vertex,
			tangentElement,
			3);
	}

	auto attributeWriter = attributeBuffer.allocate(vertexElement->numElements);
	for(std::size_t vertexIdx = 0; vertexIdx < vertexElement->numElements; ++vertexIdx)
	{
		const math::Vector3D position(
			xValues.get(vertexIdx), 
			yValues.get(vertexIdx), 
			zValues.get(vertexIdx));

		attributeWriter.setAttribute(EPrimitiveAttribute::Position_0, vertexIdx, math::Vector3R(position));
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

			attributeWriter.setAttribute(EPrimitiveAttribute::Normal_0, vertexIdx, math::Vector3R(normal));
		}
	}

	if(hasTangents)
	{
		for(std::size_t vertexIdx = 0; vertexIdx < vertexElement->numElements; ++vertexIdx)
		{
			math::Vector3D tangent(
				txValues.get(vertexIdx),
				tyValues.get(vertexIdx),
				tzValues.get(vertexIdx));

			tangent.normalizeLocal();

			uint32 tangentSignBit = 0;
			if(hasTangentSign)
			{
				const double tangentSign = twValues.get(vertexIdx);
				if(!std::isfinite(tangentSign) || tangentSign == 0.0)
				{
					PH_LOG(GPlyPolygonMesh, Debug,
						"tangent sign (tw) {} at vertex {} is zero or non-finite", tangentSign, vertexIdx);
				}
				tangentSignBit = tangentSign < 0 ? 1u : 0u;
			}

			attributeWriter.setAttribute(
				tangentAttribute,
				vertexIdx,
				math::Vector3R(tangent),
				shouldStoreTangentSign ? &tangentSignBit : nullptr);
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

	if(bakedTransform)
	{
		applyBakedTransform(attributeBuffer, attributeWriter, *bakedTransform);
	}

	// Log some stats for performance analysis
	const auto bufferMemoryUsage = loadedBuffer.memoryUsage(attributeWriter);
	const auto numFaces = loadedBuffer.numFaces();
	const auto averageFaceMemoryUsage = loadedBuffer.averagePerPolygonMemoryUsage(attributeWriter);
	PH_LOG(GPlyPolygonMesh, Note,
		"{} buffer stats: {} verts, {} faces ({:.3f} MiB, {:.3f} B per face)",
		m_plyFile.getIdentifier(),
		attributeWriter.numVertices(),
		numFaces,
		math::bytes_to_MiB<double>(bufferMemoryUsage),
		averageFaceMemoryUsage);

	return loadedBuffer;
}

IndexedTriangleBuffer GPlyPolygonMesh::loadStandardTriangleBuffer(
	const StaticAffineTransform* const bakedTransform) const
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
		"tx",
		"ty",
		"tz",
		"tw",
		"face",
		"vertex_indices",
		bakedTransform);
}

}// end namespace ph
