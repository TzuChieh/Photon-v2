#pragma once

#include "Core/Intersection/DataStructure/TAosTriMesh.h"

#include <Common/assertion.h>

#include <utility>
#include <limits>

namespace ph
{

template<
	typename Index, 
	typename Vertex, 
	typename VerticesToTriFace>
inline TAosTriMesh<Index, Vertex, VerticesToTriFace>::

TAosTriMesh(
	std::vector<Vertex> vertices,
	std::vector<Index>  indices,
	VerticesToTriFace   verticesToTriFace) : 

	TAosTriMesh(
		vertices.date(),
		vertices.size(),
		indices.data(),
		indices.size(),
		std::move(verticesToTriFace))
{}

template<
	typename Index, 
	typename Vertex, 
	typename VerticesToTriFace>
inline TAosTriMesh<Index, Vertex, VerticesToTriFace>::

TAosTriMesh(
	const Vertex* const vertices,
	const std::size_t   numVertices,
	const Index* const  indices,
	const std::size_t   numIndices,
	VerticesToTriFace   verticesToTriFace) : 

	IndexedTriMesh(numIndices / 3),

	m_verticesToTriFace(std::move(verticesToTriFace))
{
	PH_ASSERT(vertices);
	PH_ASSERT_GT(numVertices, 0);
	PH_ASSERT_LE(numVertices - 1, std::numeric_limits<Index>::max());
	PH_ASSERT(indices);
	PH_ASSERT_GT(numIndices, 0);

	m_numVertices = numVertices;
	m_numIndices  = numIndices;

	m_vertices = std::make_unique<Vertex[]>(new Vertex[numVertices]);
	for(std::size_t i = 0; i < numVertices; ++i)
	{
		m_vertices[i] = vertices[i];
	}
	
	m_indices = std::make_unique<Index[]>(new Index[numIndices]);
	for(std::size_t i = 0; i < numIndices; ++i)
	{
		PH_ASSERT_IN_RANGE_INCLUSIVE(indices[i], Index(0), std::numeric_limits<Index>::max());

		m_indices[i] = indices[i];
	}
}

template<
	typename Index, 
	typename Vertex, 
	typename VerticesToTriFace>
inline auto TAosTriMesh<Index, Vertex, VerticesToTriFace>::

getFace(const std::size_t faceIndex) const
-> TriFace
{
	PH_ASSERT_LT(faceIndex, numFaces());

	const std::size_t indicesIndex = faceIndex * 3;
	PH_ASSERT_LT(indicesIndex + 2, m_numIndices);

	return m_verticesToTriFace(
		faceIndex,
		m_vertices[m_indices[indicesIndex]],
		m_vertices[m_indices[indicesIndex + 1]], 
		m_vertices[m_indices[indicesIndex + 2]]);
}

}// end namespace ph
