#include "Actor/Geometry/GPlyPolygonMesh.h"
#include "Core/Intersectable/DataStructure/TIndexedPolygonBuffer.h"
#include "Actor/Basic/exceptions.h"
#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "Core/Intersectable/DataStructure/IndexedVertexBuffer.h"
#include "Core/Intersectable/DataStructure/IndexedUIntBuffer.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Core/Intersectable/TPIndexedKdTreeTriangleMesh.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(GPlyPolygonMesh, Geometry);

void GPlyPolygonMesh::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	IndexedTriangleBuffer* triangleBuffer = ctx.getResources()->makeTriangleBuffer();
	*triangleBuffer = loadTriangleBuffer();

	// TODO: more index types
	auto* kdTreeMesh = ctx.getResources()->makeIntersectable<TPIndexedKdTreeTriangleMesh<uint32>>(
		triangleBuffer);

	out_geometry.primitives.push_back(kdTreeMesh);
	out_geometry.triangleBuffer = triangleBuffer;
}

void GPlyPolygonMesh::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	// TODO
}

std::shared_ptr<Geometry> GPlyPolygonMesh::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	// TODO
	return nullptr;
}

IndexedTriangleBuffer GPlyPolygonMesh::loadTriangleBuffer() const
{
	PH_LOG(GPlyPolygonMesh, "loading file {}", m_filePath);

	PlyFile file(m_filePath);

	PlyElement* vertexElement = file.findElement("vertex");
	if(!vertexElement)
	{
		throw CookException("cannot find PLY vertex element");
	}

	PlyElement* faceElement = file.findElement("face");
	if(!faceElement)
	{
		throw CookException("cannot find PLY face element");
	}

	IndexedTriangleBuffer loadedBuffer;
	IndexedVertexBuffer& vertexBuffer = loadedBuffer.getVertexBuffer();
	IndexedUIntBuffer& indexBuffer = loadedBuffer.getIndexBuffer();

	// Loading vertices

	auto xValues = vertexElement->propertyValues(vertexElement->findProperty("x"));
	auto yValues = vertexElement->propertyValues(vertexElement->findProperty("y"));
	auto zValues = vertexElement->propertyValues(vertexElement->findProperty("z"));
	auto nxValues = vertexElement->propertyValues(vertexElement->findProperty("nx"));
	auto nyValues = vertexElement->propertyValues(vertexElement->findProperty("ny"));
	auto nzValues = vertexElement->propertyValues(vertexElement->findProperty("nz"));

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
	for(std::size_t vi = 0; vi < vertexElement->numElements; ++vi)
	{
		const auto x = static_cast<real>(xValues.get(vi));
		const auto y = static_cast<real>(yValues.get(vi));
		const auto z = static_cast<real>(zValues.get(vi));

		vertexBuffer.setAttribute(EVertexAttribute::Position_0, vi, {x, y, z});
	}

	if(hasNormals)
	{
		for(std::size_t vertexIdx = 0; vertexIdx < vertexElement->numElements; ++vertexIdx)
		{
			const auto nx = static_cast<real>(nxValues.get(vertexIdx));
			const auto ny = static_cast<real>(nyValues.get(vertexIdx));
			const auto nz = static_cast<real>(nzValues.get(vertexIdx));

			vertexBuffer.setAttribute(EVertexAttribute::Normal_0, vertexIdx, {nx, ny, nz});
		}
	}

	// Loading indices

	auto vertexIndexProperty = faceElement->findProperty("vertex_indices");
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

}// end namespace ph
